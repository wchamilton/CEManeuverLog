#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFileDialog>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include <QDebug>
#include <QSettings>
#include <QStandardItemModel>
#include <QComboBox>
#include <QPushButton>
#include <QActionGroup>

#include "models/PlaneModel.h"
#include "models/TurnModel.h"
#include "graphics/ManeuverScene.h"
#include "graphics/AltCtrlScene.h"
#include "graphics/FiringArcScene.h"
#include "editor/PlaneEditor.h"
#include "PreGamePrompt.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->actionQuit->setShortcut(Qt::ControlModifier+Qt::Key_Q);
    ui->turn_log->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // Init the models
    plane_model = new PlaneModel(this);

    // Init maneuver proxy to contain only maneuvers
    maneuver_proxy_model = new PlaneFilterProxy(plane_model, this);
    maneuver_proxy_model->setTypeFilter({BaseItem::Maneuver_Item_Type});

    // Init crew proxy to contain crew and their weaponry (if applicable)
    crew_proxy_model = new PlaneFilterProxy(plane_model, this);
    crew_proxy_model->setTypeFilter({BaseItem::Crew_Item_Type, BaseItem::Gun_Item_Type});

    // Init turn model which keeps a history of all events that happened during the game for tracking game logic
    turn_model = new TurnModel(this);
    ui->turn_log->setModel(turn_model);

    // Set up the damage trackers
    ui->engine_grp->setTitle("Engine");
    ui->wing_grp->setTitle("Wing");
    ui->fuselage_grp->setTitle("Fuselage");
    ui->tail_grp->setTitle("Tail");

    // Set up menu bar submenus and initialize the action group to contain all the planes
    early_war_menu = ui->select_plane_menu->addMenu("Early War");
    late_war_menu = ui->select_plane_menu->addMenu("Late War");
    plane_action_group = new QActionGroup(this);

    // Init the scene that contains the maneuver schedule
    maneuver_scene = new ManeuverScene(maneuver_proxy_model, ui->graphicsView);
    maneuver_scene->applyScheduleBG();
    maneuver_scene->positionManeuvers();
    ui->graphicsView->setScene(maneuver_scene);

    // Init the secondary control scenes which displays the alt and firing arcs
    alt_ctrl_scene = new AltCtrlScene(ui->alt_control);
    ui->alt_control->setScene(alt_ctrl_scene);
    firing_arc_scene = new FiringArcScene(ui->firing_arc_control);
    ui->firing_arc_control->setScene(firing_arc_scene);

    connect(ui->rotate_gun_left, &QPushButton::clicked, this, [=](){ rotateSelectedFlexibleGun(-1); });
    connect(ui->rotate_gun_right, &QPushButton::clicked, this, [=](){ rotateSelectedFlexibleGun(1); });

    // Load the planes if a location has been already set
    autoLoadPlanes();

    connect(ui->actionLoad_Planes, &QAction::triggered, this, [&]{
        QString file_path = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("JSON files (*.json)"));
        loadJSON(file_path);
        plane_action_group->actions().last()->trigger();
    });

    connect(ui->actionCreate_plane, &QAction::triggered, this, [&]{
        PlaneEditor(this).exec();
    });

    connect(ui->actionEdit_plane, &QAction::triggered, this, [&] {
        QString file_path = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("JSON files (*.json)"));
        if (file_path != "") {
            QFile file(file_path);
            if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
                qWarning() << "Could not open" << file_path;
                return;
            }
            QJsonDocument planes_doc = QJsonDocument::fromJson(QString(file.readAll()).toUtf8());
            file.close();
            PlaneEditor(planes_doc.object(), this).exec();
        }
    });

    connect(ui->actionSet_Plane_Auto_Load_Location, &QAction::triggered, this, [&] {
        QSettings settings;
        settings.beginGroup("CanvasEagles");
        QString load_location = "./Planes";
        if (settings.contains("auto-load_location")) {
            load_location = settings.value("auto-load_location").toString();
        }
        load_location = QFileDialog::getExistingDirectory(this, tr("Select Folder"), load_location);
        if (load_location != "") {
            settings.setValue("auto-load_location", load_location);
            autoLoadPlanes();
        }
        settings.endGroup();
    });

    connect(ui->log_movement_btn, &QPushButton::clicked, this, [=](){ setTurnState(Movement_Locked); });
    connect(ui->next_turn_btn, &QPushButton::clicked, this, [=](){
        QList<QPair<QPersistentModelIndex, QString>> crew_actions;
        QPersistentModelIndex pilot;
        for (auto control : crew_control_widgets) {
            crew_actions << control->getChosenCrewAction();
            QPersistentModelIndex crew = control->getChosenCrewAction().first;
            if (crew.sibling(crew.row(), CrewItem::Wounds).data().toInt() < 3 &&
                    crew.sibling(crew.row(), CrewItem::Crew_Role).data().toString() == "Pilot") {
                pilot = crew.sibling(crew.row(), CrewItem::Has_Unrestricted_Maneuvers);
            }
        }

        turn_model->addTurn(maneuver_scene->getSelectedManeuverIdx(), alt_ctrl_scene->getCurrentAlt(), crew_actions);
        ui->turn_log->expandAll();
        for (int i=0; i<ui->turn_log->header()->count(); ++i) {
            ui->turn_log->resizeColumnToContents(i);
        }

        maneuver_scene->clearSelection();
        maneuver_scene->setManeuversAvailable(pilot.isValid() ? pilot.data().toBool() : false);
        alt_ctrl_scene->setManeuver(QModelIndex());
        setTurnState(Start_Of_Turn);
    });
    connect(maneuver_scene, &ManeuverScene::maneuverClicked, this, [=](QPersistentModelIndex idx) {
        alt_ctrl_scene->setManeuver(idx);
        setTurnState(Movement_Selected);
    });
}

MainWindow::~MainWindow()
{
    maneuver_scene->clearSelection();
    delete ui;
}

void MainWindow::loadJSON(QString file_path)
{
    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        qWarning() << "Could not open" << file_path;
        return;
    }
    QJsonDocument planes_doc = QJsonDocument::fromJson(QString(file.readAll()).toUtf8());
    file.close();

    generatePlaneMenu(plane_model->loadPlaneJSON(planes_doc.object()));
    ui->statusbar->showMessage(tr("%1 planes loaded").arg(plane_model->rowCount()));
}

void MainWindow::setSelectedPlane()
{
    // Start by ensuring we've got a clean slate
    clearUI();

    QPersistentModelIndex plane_idx = plane_action_group->checkedAction()->data().toPersistentModelIndex();
    maneuver_scene->setPlane(maneuver_proxy_model->mapFromSource(plane_idx));

    PreGamePrompt crew_names_dlg(crew_proxy_model, crew_proxy_model->mapFromSource(plane_idx), turn_model, this);
    crew_names_dlg.exec();

    maneuver_scene->setTurnModel(turn_model);
    alt_ctrl_scene->setTurnModel(turn_model);
    alt_ctrl_scene->setPlane(maneuver_proxy_model->mapFromSource(plane_idx));

    ui->actionSet_up_Crew->setEnabled(true);

    // Iterate over the crew members
    for (int i=0; i<crew_proxy_model->rowCount(crew_proxy_model->mapFromSource(plane_idx)); ++i) {
        QPersistentModelIndex crew_idx = crew_proxy_model->index(i, CrewItem::Crew_Name, crew_proxy_model->mapFromSource(plane_idx));
        CrewControls* cc = new CrewControls(crew_proxy_model, crew_idx, turn_model, ui->crew_tab);
        ui->crew_tab->addTab(cc, crew_idx.sibling(crew_idx.row(), CrewItem::Crew_Role).data().toString() + " (" + crew_idx.data().toString() + ")");
        crew_control_widgets.insert(crew_idx.data().toString(), cc);
//        connect(cc, &CrewControls::updateSelectedAction, this, &MainWindow::updateCurrentTurn);

        // Add each of the guns to the firing arc combobox
        for (int i=0; i<crew_proxy_model->rowCount(crew_idx); ++i) {
            QPersistentModelIndex gun_idx = crew_proxy_model->index(i, GunItem::Gun_Name, crew_idx);
            ui->firing_arc_selection->addItem(QString("%1 (%2)").arg(gun_idx.data().toString()).arg(crew_idx.data().toString()), gun_idx);
        }
    }
    connect(ui->firing_arc_selection, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=]() {
        QPersistentModelIndex gun_idx = ui->firing_arc_selection->currentData().toPersistentModelIndex();
        firing_arc_scene->setCurrentGun(gun_idx);
        ui->gun_pos_spin->setValue(gun_idx.sibling(gun_idx.row(), GunItem::Gun_Position).data().toInt());
    });
    ui->firing_arc_selection->currentIndexChanged(0);

    // Set the module HP indexes
    ui->engine_grp->setModelIndexes(plane_idx.sibling(plane_idx.row(), PlaneItem::Engine_HP), plane_idx.sibling(plane_idx.row(), PlaneItem::Engine_Critical));
    ui->wing_grp->setModelIndexes(plane_idx.sibling(plane_idx.row(), PlaneItem::Wing_HP), plane_idx.sibling(plane_idx.row(), PlaneItem::Wing_Critical));
    ui->fuselage_grp->setModelIndexes(plane_idx.sibling(plane_idx.row(), PlaneItem::Fuselage_HP), plane_idx.sibling(plane_idx.row(), PlaneItem::Fuselage_Critical));
    ui->tail_grp->setModelIndexes(plane_idx.sibling(plane_idx.row(), PlaneItem::Tail_HP), plane_idx.sibling(plane_idx.row(), PlaneItem::Tail_Critical));

    // Iterate over the crew members
    for (int i=0; i<crew_proxy_model->rowCount(crew_proxy_model->mapFromSource(plane_idx)); ++i) {
        QPersistentModelIndex crew_idx = crew_proxy_model->index(i, CrewItem::Crew_Name, crew_proxy_model->mapFromSource(plane_idx));
//        QTreeWidgetItem* child = new QTreeWidgetItem({crew_idx.data().toString()});
    }

    setTurnState(Start_Of_Turn);
}

void MainWindow::rotateSelectedFlexibleGun(int delta)
{
    QPersistentModelIndex gun_idx = ui->firing_arc_selection->currentData().toPersistentModelIndex();
    if (!gun_idx.isValid()) {
        return;
    }

    int starting_pos = gun_idx.sibling(gun_idx.row(), GunItem::Gun_Last_Position).data().toInt();
    QList<int> range = gun_idx.sibling(gun_idx.row(), GunItem::Gun_Position_Range).data().value<QList<int>>();
    int current_range_idx = range.indexOf(ui->gun_pos_spin->value());
    int starting_pos_idx = range.indexOf(starting_pos);
    if ((current_range_idx == starting_pos_idx+1 || current_range_idx == starting_pos_idx+7) && delta == 1) {
        return;
    }
    else if ((current_range_idx == starting_pos_idx-1 || current_range_idx == starting_pos_idx+5) && delta == -1) {
        return;
    }

    // Handle wrap arounds
    if (current_range_idx == 0 && delta == -1) {
        ui->gun_pos_spin->setValue(range.at(range.size()-1));
    }
    else if (current_range_idx == range.size()-1 && delta == 1) {
        ui->gun_pos_spin->setValue(range.at(0));
    }
    else {
        ui->gun_pos_spin->setValue(range.at(current_range_idx + delta));
    }

    firing_arc_scene->setGunRotation(ui->gun_pos_spin->value());
    crew_proxy_model->setData(gun_idx.sibling(gun_idx.row(), GunItem::Gun_Position), ui->gun_pos_spin->value());
}

void MainWindow::setTurnState(MainWindow::TurnState state)
{
    ui->graphicsView->setEnabled(state != Movement_Locked);
    ui->firing_arc_control->setEnabled(state == Movement_Selected);
    ui->alt_control->setEnabled(state == Movement_Selected);
    ui->rotate_gun_left->setEnabled(state == Movement_Selected);
    ui->rotate_gun_right->setEnabled(state == Movement_Selected);
    ui->log_movement_btn->setEnabled(state == Movement_Selected);
    ui->crew_tab->setEnabled(state == Movement_Locked);
    ui->next_turn_btn->setEnabled(state == Movement_Locked);
}

void MainWindow::autoLoadPlanes()
{
    QSettings settings;
    settings.beginGroup("CanvasEagles");
    QString load_location = settings.contains("auto-load_location") ? settings.value("auto-load_location").toString() : "./Planes";
    settings.endGroup();
    if (load_location != "") {
        early_war_menu->clear();
        late_war_menu->clear();
        QDir planes_dir(load_location);
        for (QFileInfo file_info : planes_dir.entryInfoList({"*.json"})) {
            loadJSON(file_info.absoluteFilePath());
        }
    }
}

void MainWindow::clearUI()
{
    maneuver_scene->setPlane(QPersistentModelIndex());

    ui->engine_grp->clear();
    ui->wing_grp->clear();
    ui->fuselage_grp->clear();
    ui->tail_grp->clear();
    ui->crew_tab->clear();
    ui->firing_arc_selection->clear();

    // Clear all the items within the crew action widget map
    qDeleteAll(crew_control_widgets);
    crew_control_widgets.clear();

    // Ensure the entire turn log is clean
    turn_model->clearModel();
}

void MainWindow::generatePlaneMenu(QPersistentModelIndex idx)
{
    QAction* plane_action = nullptr;
    if (idx.sibling(idx.row(), PlaneItem::Plane_Era).data().toString() == "Early War") {
        plane_action = early_war_menu->addAction(idx.data().toString());
    }
    else if (idx.sibling(idx.row(), PlaneItem::Plane_Era).data().toString() == "Late War") {
        plane_action = late_war_menu->addAction(idx.data().toString());
    }
    plane_action->setData(idx);
    plane_action->setCheckable(true);
    plane_action_group->addAction(plane_action);
    connect(plane_action, &QAction::triggered, this, &MainWindow::setSelectedPlane);
}
