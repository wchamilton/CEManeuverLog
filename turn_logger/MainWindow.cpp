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
    crew_proxy_model->expandFilter({BaseItem::Crew_Item_Type, BaseItem::Gun_Item_Type});

    // Set up the damage trackers
    ui->engine_grp->setTitle("Engine");
    ui->wing_grp->setTitle("Wing");
    ui->fuselage_grp->setTitle("Fuselage");
    ui->tail_grp->setTitle("Tail");

    early_war_menu = ui->select_plane_menu->addMenu("Early War");
    late_war_menu = ui->select_plane_menu->addMenu("Late War");
    plane_action_group = new QActionGroup(this);

    maneuver_scene = new ManeuverScene(ui->graphicsView);
    maneuver_scene->applyScheduleBG();
    maneuver_scene->positionManeuvers();
    ui->graphicsView->setScene(maneuver_scene);

    maneuver_preview_scene = new ManeuverScene(ui->selected_maneuver_graphic);
    ui->selected_maneuver_graphic->setScene(maneuver_preview_scene);

    // Load the planes if a location has been already set
    autoLoadPlanes();
    ui->maneuver_cmb->setCurrentIndex(-1);

    connect(ui->actionLoad_Planes, &QAction::triggered, this, [&]{
        QString file_path = QFileDialog::getOpenFileName(this, tr("Open File"), "/home", tr("JSON files (*.json)"));
        ui->maneuver_cmb->blockSignals(true);
        loadJSON(file_path);
        ui->maneuver_cmb->blockSignals(false);
        plane_action_group->actions().last()->trigger();
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
        settings.beginGroup("CanvasEagles");
        QString load_location = "/home";
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

    connect(maneuver_scene, &ManeuverScene::selectionChanged, this, [=] {
        setManeuver(maneuver_scene->getSelectedManeuver());
    });
    connect(ui->maneuver_cmb, &QComboBox::currentTextChanged, this, &MainWindow::setManeuver);
    connect(ui->log_turn_btn, &QPushButton::released, this, &MainWindow::logMovement);
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
    ui->turn_controls_grp->setEnabled(true);

    QPersistentModelIndex plane_idx = plane_action_group->checkedAction()->data().toPersistentModelIndex();
    maneuver_scene->setManeuvers(maneuver_proxy_model->mapFromSource(plane_idx));
    maneuver_scene->update();
    ui->maneuver_cmb->setEnabled(true);
    ui->maneuver_cmb->setRootModelIndex(maneuver_proxy_model->mapFromSource(plane_idx));
    ui->maneuver_cmb->setCurrentIndex(-1);

    ui->turn_log->clear();
    ui->crew_tab->clear();

    // Iterate over the crew members
    for (int i=0; i<crew_proxy_model->rowCount(crew_proxy_model->mapFromSource(plane_idx)); ++i) {
        QPersistentModelIndex crew_idx = crew_proxy_model->index(i, CrewItem::Crew_Role, crew_proxy_model->mapFromSource(plane_idx));
        ui->crew_tab->addTab(new CrewControls(crew_proxy_model, crew_idx, ui->crew_tab), crew_idx.data().toString());
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

    addTurn();
}

void MainWindow::logMovement()
{
    auto idx = plane_action_group->checkedAction()->data().toPersistentModelIndex();

    int fuel_used = 0;
    for (auto t : turn_history) {
        fuel_used += t.fuel_used;
    }
    int max_fuel = idx.sibling(idx.row(), PlaneItem::Fuel).data().toInt();
    int fuel_remaining = max_fuel - fuel_used >= 0 ? max_fuel - fuel_used : 0;

    QTreeWidgetItem* log_item_maneuver = ui->turn_log->topLevelItem(ui->turn_log->topLevelItemCount() -2); // newest item is occupied by the add button
    log_item_maneuver->setText(0, QString("Turn %1 - %2").arg(ui->turn_log->topLevelItemCount()-1).arg(ui->maneuver_cmb->currentText()));
    log_item_maneuver->setText(1, ui->alt_cmb->currentText());
    log_item_maneuver->setText(2, QString("%1/%2").arg(fuel_remaining).arg(max_fuel));
    // Calculate if below 25% fuel remaining
    if (fuel_remaining <= max_fuel * 0.25) {
        log_item_maneuver->setForeground(2, Qt::red);
    }
    else if (fuel_remaining <= max_fuel * 0.5) {
        log_item_maneuver->setForeground(2, QColor(255,80,50));
    }
//    ui->turn_log->addTopLevelItem(log_item_maneuver);
//    ui->turn_log->scrollToItem(log_item_maneuver);
}

void MainWindow::logCrewAction()
{
    // Grab the currently selected crew and apply the action taken. This should also overwrite a previous one in case
    // the user changes their mind during the turn
}

void MainWindow::setManeuver(QString maneuver_name)
{
    // If selection was done from scene, update combobox
    if (sender() == ui->maneuver_cmb && maneuver_scene != nullptr){
        maneuver_scene->blockSignals(true);
        if (!maneuver_scene->selectedItems().isEmpty()) {
            maneuver_scene->clearSelection();
        }
        maneuver_scene->getManeuver(maneuver_name)->setSelected(true);
        maneuver_scene->blockSignals(false);
    }
    // If selection was done from combobox, update scene
    else if (sender() == maneuver_scene && ui->maneuver_cmb != nullptr) {
        ui->alt_cmb->setDisabled(maneuver_name == "");
        ui->log_turn_btn->setDisabled(maneuver_name == "");

        if (ui->maneuver_cmb->currentText() != maneuver_name) {
            ui->maneuver_cmb->blockSignals(true);
            ui->maneuver_cmb->setCurrentIndex(ui->maneuver_cmb->findText(maneuver_name));
            ui->maneuver_cmb->blockSignals(false);
        }
    }

    // If the name is blank then assume selection cleared, otherwise update the preview with the maneuver
    if (maneuver_preview_scene != nullptr) {
        if (maneuver_name == "") {
            maneuver_preview_scene->setManeuver(QPersistentModelIndex());
            maneuver_preview_scene->update();
        }
        else {
            QPersistentModelIndex idx(maneuver_proxy_model->index(ui->maneuver_cmb->currentIndex(), ManeuverItem::Maneuver_Name, ui->maneuver_cmb->rootModelIndex()));
            maneuver_preview_scene->setManeuver(idx);
            ui->selected_maneuver_graphic->fitInView(maneuver_preview_scene->getManeuver(idx.data().toString()), Qt::KeepAspectRatio);
        }
    }

    setAvailableAltitudes();
}

void MainWindow::addTurn()
{
    if (ui->turn_log->topLevelItemCount() == 0) {
        QPushButton* next_turn_btn = new QPushButton(tr("Start Turn %1").arg(ui->turn_log->topLevelItemCount() + 1));

        QTreeWidgetItem* next_turn_btn_item = new QTreeWidgetItem(ui->turn_log);
        next_turn_btn_item->setFirstColumnSpanned(true);
        ui->turn_log->setItemWidget(next_turn_btn_item, 0, next_turn_btn);

        connect(next_turn_btn, &QPushButton::clicked, this, [&]() {
            QTreeWidgetItem* btn_item = ui->turn_log->currentItem();
            QTreeWidgetItem* turn_item = new QTreeWidgetItem({tr("Turn %1 - ?").arg(ui->turn_log->topLevelItemCount()), "---", "---"});
            ui->turn_log->insertTopLevelItem(ui->turn_log->topLevelItemCount()-1, turn_item);
            ui->turn_log->addTopLevelItem(btn_item);
            static_cast<QPushButton*>(sender())->setText(tr("Start Turn %1").arg(ui->turn_log->topLevelItemCount()));

            QPersistentModelIndex plane_idx = plane_action_group->checkedAction()->data().toPersistentModelIndex();
            // Iterate over the crew members
            for (int i=0; i<crew_proxy_model->rowCount(crew_proxy_model->mapFromSource(plane_idx)); ++i) {
                QPersistentModelIndex crew_idx = crew_proxy_model->index(i, CrewItem::Crew_Role, crew_proxy_model->mapFromSource(plane_idx));
                turn_item->addChild(new QTreeWidgetItem({crew_idx.data().toString()}));
            }
            turn_item->setExpanded(true);

            TurnData td;
            td.maneuver = ui->maneuver_cmb->currentText();
            td.alt = ui->alt_cmb->currentText().toInt();
            td.fuel_used = calculateFuelUsed();

            turn_history << td;

            setAvailableAltitudes();
        });
    }
}

void MainWindow::autoLoadPlanes()
{
    QSettings settings;
    settings.beginGroup("CanvasEagles");
    QString load_location = settings.contains("auto-load_location") ? settings.value("auto-load_location").toString() : "";
    settings.endGroup();
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
