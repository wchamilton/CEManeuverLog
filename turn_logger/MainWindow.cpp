#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFileDialog>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include <QDebug>
#include <QDesktopWidget>
#include <QSettings>
#include <QStandardItemModel>

#include "models/PlaneModel.h"
#include "graphics/ManeuverScene.h"
#include "graphics/ManeuverGraphic.h"
#include "editor/PlaneEditor.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->actionQuit->setShortcut(Qt::ControlModifier+Qt::Key_Q);
    ui->turn_log->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // Init the models
    plane_model = new PlaneModel(this);

    maneuver_proxy_model = new PlaneFilterProxy(plane_model, this);
    maneuver_proxy_model->expandFilter(BaseItem::Maneuver_Item_Type);
    ui->maneuver_cmb->setModel(maneuver_proxy_model);

    crew_proxy_model = new PlaneFilterProxy(plane_model, this);
    crew_proxy_model->expandFilter(BaseItem::Crew_Item_Type);
    crew_proxy_model->expandFilter(BaseItem::Gun_Item_Type);

    // Set up the damage trackers
    ui->engine_grp->setTitle("Engine");
    ui->wing_grp->setTitle("Wing");
    ui->fuselage_grp->setTitle("Fuselage");
    ui->tail_grp->setTitle("Tail");

    early_war_menu = ui->select_plane_menu->addMenu("Early War");
    late_war_menu = ui->select_plane_menu->addMenu("Late War");
    plane_action_group = new QActionGroup(this);

    maneuver_scene = new ManeuverScene(ui->graphicsView);
    maneuver_scene->positionManeuvers();
    ui->graphicsView->setScene(maneuver_scene);
    ui->graphicsView->setRenderHints(QPainter::Antialiasing);

    // Load the planes if a location has been already set
    autoLoadPlanes();
    ui->maneuver_cmb->setCurrentIndex(-1);

    connect(ui->actionLoad_Planes, &QAction::triggered, this, [&]{
        QString file_path = QFileDialog::getOpenFileName(this, tr("Open File"), "/home", tr("JSON files (*.json)"));
        loadJSON(file_path);
    });

    connect(ui->actionCreate_plane, &QAction::triggered, this, [&]{
        PlaneEditor(this).exec();
    });

    connect(ui->actionEdit_plane, &QAction::triggered, this, [&] {
        QString file_path = QFileDialog::getOpenFileName(this, tr("Open File"), "/home", tr("JSON files (*.json)"));
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
        QString load_location = "/home";
        if (settings.contains("auto-load_location")) {
            load_location = settings.value("auto-load_location").toString();
        }
        load_location = QFileDialog::getExistingDirectory(this, tr("Select Folder"), load_location);
        if (load_location != "") {
            settings.setValue("auto-load_location", load_location);
            autoLoadPlanes();
        }
    });

    connect(maneuver_scene, &ManeuverScene::maneuverSelectionChanged, ui->maneuver_cmb, [&](QString name){
        ui->maneuver_cmb->blockSignals(true);
        ui->maneuver_cmb->setCurrentIndex(ui->maneuver_cmb->findText(name));
        ui->alt_cmb->setDisabled(name == "");
        ui->log_turn_btn->setDisabled(name == "");
        setAvailableAltitudes();
        ui->maneuver_cmb->blockSignals(false);
    });
    connect(ui->maneuver_cmb, &QComboBox::currentTextChanged, maneuver_scene, [&](QString name){
        maneuver_scene->clearSelection();
        ui->alt_cmb->setDisabled(name == "");
        ui->log_turn_btn->setDisabled(name == "");
        if (name != "") {
            maneuver_scene->getManeuver(name)->setSelected(true);
            setAvailableAltitudes();
        }
    });
    connect(ui->log_turn_btn, &QPushButton::released, this, &MainWindow::logTurn);
}

MainWindow::~MainWindow()
{
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
    ui->turn_controls_grp->setEnabled(true);

    QPersistentModelIndex plane_idx = plane_action_group->checkedAction()->data().toPersistentModelIndex();
    maneuver_scene->setManeuvers(maneuver_proxy_model->mapFromSource(plane_idx));
    maneuver_scene->update();
    ui->maneuver_cmb->setEnabled(true);
    ui->maneuver_cmb->setRootModelIndex(maneuver_proxy_model->mapFromSource(plane_idx));
    ui->maneuver_cmb->setCurrentIndex(-1);

    ui->turn_log->clear();
    turn_history.clear();

    // Populate the crew tab and the gun tabs for each crew
    ui->crew_tab->clear();
    ui->gun_tab->clear();
    for (int i=0; i<crew_proxy_model->rowCount(crew_proxy_model->mapFromSource(plane_idx)); ++i) {
        QPersistentModelIndex crew_idx = crew_proxy_model->index(i, CrewItem::Crew_Role, crew_proxy_model->mapFromSource(plane_idx));
        ui->crew_tab->addTab(new CrewControls(ui->crew_tab), crew_idx.data().toString());

        for (int j=0; j<crew_proxy_model->rowCount(crew_idx); ++j) {
            QPersistentModelIndex gun_idx = crew_proxy_model->index(j, GunItem::Gun_Name, crew_idx);
            QString gun_prefix = "";
            int gun_count = gun_idx.sibling(gun_idx.row(), GunItem::Gun_Count).data().toInt();
            if (gun_count == 2) {
                gun_prefix = "Twin ";
            }
            else if (gun_count == 3) {
                gun_prefix = "Triple ";
            }
            ui->gun_tab->addTab(new GunControls(gun_idx, ui->gun_tab),
                                QString("%1's %2%3").arg(crew_idx.data().toString()).arg(gun_prefix).arg(gun_idx.data().toString()));
        }
    }

    // Set the module HP indexes
    ui->engine_grp->setModelIndexes(plane_idx.sibling(plane_idx.row(), PlaneItem::Engine_HP), plane_idx.sibling(plane_idx.row(), PlaneItem::Engine_Critical));
    ui->wing_grp->setModelIndexes(plane_idx.sibling(plane_idx.row(), PlaneItem::Wing_HP), plane_idx.sibling(plane_idx.row(), PlaneItem::Wing_Critical));
    ui->fuselage_grp->setModelIndexes(plane_idx.sibling(plane_idx.row(), PlaneItem::Fuselage_HP), plane_idx.sibling(plane_idx.row(), PlaneItem::Fuselage_Critical));
    ui->tail_grp->setModelIndexes(plane_idx.sibling(plane_idx.row(), PlaneItem::Tail_HP), plane_idx.sibling(plane_idx.row(), PlaneItem::Tail_Critical));

    // Countdown rather than count up
    for (int i=plane_idx.sibling(plane_idx.row(), PlaneItem::Max_Altitude).data().toInt(); i>0; --i) {
        ui->alt_cmb->addItem(QString::number(i));
    }
    ui->turn_log->setEnabled(true);
}

void MainWindow::logTurn()
{
    auto idx = plane_action_group->checkedAction()->data().toPersistentModelIndex();

    TurnData turn;
    turn.maneuver = ui->maneuver_cmb->currentText();
    turn.alt = ui->alt_cmb->currentText().toInt();
    turn.fuel_used = calculateFuelUsed();
//    for (int i=0; i<crew_proxy_model->rowCount(crew_proxy_model->mapFromSource(idx)); ++i) {
//        crew_proxy_model->index(i, )
//    }
    turn_history << turn;

    int fuel_used = 0;
    for (auto t : turn_history) {
        fuel_used += t.fuel_used;
    }
    int max_fuel = idx.sibling(idx.row(), PlaneItem::Fuel).data().toInt();
    int fuel_remaining = max_fuel - fuel_used >= 0 ? max_fuel - fuel_used : 0;

    QTreeWidgetItem* log_item_maneuver = new QTreeWidgetItem(ui->turn_log);
    log_item_maneuver->setText(0, QString("Turn %1 - %2").arg(turn_history.size()).arg(turn.maneuver));
    log_item_maneuver->setText(1, ui->alt_cmb->currentText());
    log_item_maneuver->setText(2, QString("%1/%2").arg(fuel_remaining).arg(max_fuel));
    // Calculate if below 25% fuel remaining
    if (fuel_remaining <= max_fuel * 0.25) {
        log_item_maneuver->setForeground(2, Qt::red);
    }
    else if (fuel_remaining <= max_fuel * 0.5) {
        log_item_maneuver->setForeground(2, QColor(255,80,50));
    }
    ui->turn_log->addTopLevelItem(log_item_maneuver);
    ui->turn_log->scrollToItem(log_item_maneuver);
    setAvailableAltitudes();
}

void MainWindow::autoLoadPlanes()
{
    QSettings settings;
    QString load_location = settings.contains("auto-load_location") ? settings.value("auto-load_location").toString() : "";
    if (load_location != "") {
        clearUI();
        QDir planes_dir(load_location);
        for (QFileInfo file_info : planes_dir.entryInfoList({"*.json"})) {
            loadJSON(file_info.absoluteFilePath());
        }
    }
}

void MainWindow::clearUI()
{
    plane_model->clearModel();

    early_war_menu->clear();
    late_war_menu->clear();

    ui->engine_grp->clear();
    ui->wing_grp->clear();
    ui->fuselage_grp->clear();
    ui->tail_grp->clear();
    ui->crew_tab->clear();
    ui->gun_tab->clear();

    maneuver_scene->setManeuvers(QPersistentModelIndex());
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

int MainWindow::calculateFuelUsed()
{
    // Baseline for fuel used is the speed of the maneuver
    int fuel_used = maneuver_proxy_model->index(ui->maneuver_cmb->currentIndex(), ManeuverItem::Speed, ui->maneuver_cmb->rootModelIndex()).data().toInt();
    // First turn should assume that the altitude is what the plane has been at already, thus just return the speed
    if (turn_history.isEmpty()) {
        return fuel_used;
    }

    // Check if the current altitude is higher than the previous turn's altitude
    bool climbing = ui->alt_cmb->currentText().toInt() > turn_history.last().alt;

    // Check for zoom climbing
    int prev_delta = 0;
    if (turn_history.size() >= 2) {
        prev_delta = turn_history.last().alt - turn_history.at(turn_history.size() - 2).alt;
    }
    bool zoom_climbing = prev_delta <= -2 && climbing;

    // If climbing, add one fuel unless zoom climbing
    return climbing ? zoom_climbing ? fuel_used : fuel_used + 1 : fuel_used;
}

void MainWindow::setAvailableAltitudes()
{
    // If it's the start of the game, don't restrict altitudes
    if (turn_history.isEmpty()) {
        return;
    }

    auto * model = qobject_cast<QStandardItemModel*>(ui->alt_cmb->model());
    if(!model) {
        return;
    }
    QString maneuver_dive_val = maneuver_proxy_model->index(ui->maneuver_cmb->currentIndex(), ManeuverItem::Dive_Value, ui->maneuver_cmb->rootModelIndex()).data().toString();
    QString maneuver_climb_val = maneuver_proxy_model->index(ui->maneuver_cmb->currentIndex(), ManeuverItem::Climb_Value, ui->maneuver_cmb->rootModelIndex()).data().toString();
    auto idx = plane_action_group->checkedAction()->data().toPersistentModelIndex();

    // Determine the minimum altitude the player can dive to
    int min = maneuver_dive_val == "D1" ? turn_history.last().alt - 1 : maneuver_dive_val == "-" ? turn_history.last().alt : 1;

    // Determine if the player can climb
    int can_climb_to = idx.sibling(idx.row(), PlaneItem::Rated_Climb).data().toInt() + turn_history.last().alt;
    int max_alt = idx.sibling(idx.row(), PlaneItem::Max_Altitude).data().toInt();

    if (!(idx.sibling(idx.row(), PlaneItem::Can_Return_To_Max_Alt).data().toBool() || turn_history.isEmpty())) {
        max_alt--;
    }
    if (maneuver_climb_val == "C1") {
        max_alt = std::min(turn_history.last().alt + 1, max_alt);
    }
    else if (maneuver_climb_val == "-") {
        max_alt = turn_history.last().alt;
    }
    else if (maneuver_climb_val == "C") {
        max_alt = std::min(max_alt, can_climb_to);
    }

    for (int i=0; i<ui->alt_cmb->count(); ++i) {
        auto * item = model->item(i);
        if(!item) {
            continue;
        }
        item->setEnabled(item->text().toInt() >= min && item->text().toInt() <= max_alt);
    }
}
