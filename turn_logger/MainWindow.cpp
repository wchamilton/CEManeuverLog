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
#include <QComboBox>
#include <QPushButton>

#include "models/PlaneModel.h"
#include "graphics/ManeuverScene.h"
#include "graphics/ManeuverGraphic.h"
#include "editor/PlaneEditor.h"
#include "CrewNamesPrompt.h"

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

    connect(maneuver_scene, &ManeuverScene::selectionChanged, this, &MainWindow::updateCurrentTurn);
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
    maneuver_scene->setManeuvers(maneuver_proxy_model->mapFromSource(plane_idx));
    maneuver_scene->update();

    CrewNamesPrompt crew_names_dlg(crew_proxy_model, crew_proxy_model->mapFromSource(plane_idx), this);
    crew_names_dlg.exec();
    ui->actionSet_up_Crew->setEnabled(true);

    if (alt_cmb == nullptr) {
        alt_cmb = new QComboBox(this);
        connect(alt_cmb, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::updateCurrentTurn);
    }

    // Iterate over the crew members
    for (int i=0; i<crew_proxy_model->rowCount(crew_proxy_model->mapFromSource(plane_idx)); ++i) {
        QPersistentModelIndex crew_idx = crew_proxy_model->index(i, CrewItem::Crew_Name, crew_proxy_model->mapFromSource(plane_idx));
        CrewControls* cc = new CrewControls(crew_proxy_model, crew_idx, ui->crew_tab);
        ui->crew_tab->addTab(cc, crew_idx.sibling(crew_idx.row(), CrewItem::Crew_Role).data().toString() + " (" + crew_idx.data().toString() + ")");
        crew_control_widgets.insert(crew_idx.data().toString(), cc);
        connect(cc, &CrewControls::updateSelectedAction, this, &MainWindow::updateCurrentTurn);
    }

    // Set the module HP indexes
    ui->engine_grp->setModelIndexes(plane_idx.sibling(plane_idx.row(), PlaneItem::Engine_HP), plane_idx.sibling(plane_idx.row(), PlaneItem::Engine_Critical));
    ui->wing_grp->setModelIndexes(plane_idx.sibling(plane_idx.row(), PlaneItem::Wing_HP), plane_idx.sibling(plane_idx.row(), PlaneItem::Wing_Critical));
    ui->fuselage_grp->setModelIndexes(plane_idx.sibling(plane_idx.row(), PlaneItem::Fuselage_HP), plane_idx.sibling(plane_idx.row(), PlaneItem::Fuselage_Critical));
    ui->tail_grp->setModelIndexes(plane_idx.sibling(plane_idx.row(), PlaneItem::Tail_HP), plane_idx.sibling(plane_idx.row(), PlaneItem::Tail_Critical));

    // Countdown rather than count up
    alt_cmb->blockSignals(true);
    for (int i=plane_idx.sibling(plane_idx.row(), PlaneItem::Max_Altitude).data().toInt(); i>0; --i) {
        alt_cmb->addItem(QString::number(i));
    }
    alt_cmb->blockSignals(false);
    ui->turn_log->setEnabled(true);

    if (current_turn_item == nullptr) {
        current_turn_item = new QTreeWidgetItem(ui->turn_log);
        ui->turn_log->setItemWidget(current_turn_item, Altitude, alt_cmb);
        current_turn_item->setExpanded(true);
    }

    // Iterate over the crew members
    for (int i=0; i<crew_proxy_model->rowCount(crew_proxy_model->mapFromSource(plane_idx)); ++i) {
        QPersistentModelIndex crew_idx = crew_proxy_model->index(i, CrewItem::Crew_Name, crew_proxy_model->mapFromSource(plane_idx));
        QTreeWidgetItem* child = new QTreeWidgetItem({crew_idx.data().toString()});
        current_turn_item->addChild(child);
        current_crew_turn_items.insert(crew_idx.data().toString(), child);
    }

    if (next_turn_btn == nullptr) {
        next_turn_btn = new QPushButton(this);
    }

    if (next_turn_btn_item == nullptr) {
        next_turn_btn_item = new QTreeWidgetItem(ui->turn_log);
        next_turn_btn_item->setFirstColumnSpanned(true);
        next_turn_btn = new QPushButton(tr("Start Turn %1").arg(ui->turn_log->topLevelItemCount()));
        ui->turn_log->setItemWidget(next_turn_btn_item, 0, next_turn_btn);

        connect(next_turn_btn, &QPushButton::clicked, this, &MainWindow::addTurn);
    }

    setAvailableManeuvers();
    updateCurrentTurn();
}

void MainWindow::updateCurrentTurn()
{
    QString maneuver_selected = maneuver_scene->getSelectedManeuver();
    // Current turn + maneuver
    current_turn_item->setText(Maneuver_Name, tr("Turn %1 - %2").arg(ui->turn_log->topLevelItemCount()-1)
                                                                .arg(maneuver_selected));

    setAvailableAltitudes();
    next_turn_btn->setDisabled(maneuver_selected == QString());

    // Current fuel usage
    auto idx = plane_action_group->checkedAction()->data().toPersistentModelIndex();
    int fuel_used = calculateFuelUsed();

    int max_fuel = idx.sibling(idx.row(), PlaneItem::Fuel).data().toInt();
    int fuel_remaining = 0;
    if (ui->turn_log->topLevelItemCount() < 3) {
        fuel_remaining = max_fuel - fuel_used;
    }
    else {
        fuel_remaining = ui->turn_log->topLevelItem(ui->turn_log->topLevelItemCount()-3)->text(Fuel_Used).split('/').first().toInt() - fuel_used;
        fuel_remaining = fuel_remaining >= 0 ? fuel_remaining : 0; // Ensure we don't display less than 0 fuel

        // Calculate if below 25% fuel remaining
        if (fuel_remaining <= max_fuel * 0.25) {
            current_turn_item->setForeground(2, Qt::red);
        }
        else if (fuel_remaining <= max_fuel * 0.5) {
            current_turn_item->setForeground(2, QColor(255,80,50));
        }
    }
    current_turn_item->setText(Fuel_Used, QString("%1/%2").arg(fuel_remaining).arg(max_fuel));

    // Crew Actions
    for (auto key : current_crew_turn_items.keys()) {
        current_crew_turn_items[key]->setText(Crew_Action, crew_control_widgets[key]->getChosenAction());
    }
}

void MainWindow::addTurn()
{
    QTreeWidgetItem* top_level_item = new QTreeWidgetItem({current_turn_item->text(Maneuver_Name),
                                                           alt_cmb->currentText(),
                                                           current_turn_item->text(Fuel_Used)});
    top_level_item->setForeground(2, current_turn_item->foreground(2));

    QPersistentModelIndex plane_idx = plane_action_group->checkedAction()->data().toPersistentModelIndex();
    // Iterate over the crew members
    for (int i=0; i<crew_proxy_model->rowCount(crew_proxy_model->mapFromSource(plane_idx)); ++i) {
        QPersistentModelIndex crew_idx = crew_proxy_model->index(i, CrewItem::Crew_Name, crew_proxy_model->mapFromSource(plane_idx));
        QString crew_name = crew_idx.data().toString();
        top_level_item->addChild(new QTreeWidgetItem({crew_name, current_crew_turn_items[crew_name]->text(Crew_Action)}));
        crew_control_widgets[crew_name]->handleTurnEnd();
    }

    ui->turn_log->insertTopLevelItem(ui->turn_log->topLevelItemCount()-2, top_level_item);
    top_level_item->setExpanded(true);
    ui->turn_log->scrollToItem(next_turn_btn_item);

    next_turn_btn->setText(tr("Start Turn %1").arg(ui->turn_log->topLevelItemCount()));

    setAvailableManeuvers();
    updateCurrentTurn();
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
    maneuver_scene->setManeuvers(QPersistentModelIndex());

    ui->engine_grp->clear();
    ui->wing_grp->clear();
    ui->fuselage_grp->clear();
    ui->tail_grp->clear();
    ui->crew_tab->clear();
    if (alt_cmb != nullptr) {
        alt_cmb->clear();
    }

    // Clear all the items within the crew action widget map
    qDeleteAll(crew_control_widgets);
    crew_control_widgets.clear();

    // Clear all the items handling the current crew turn
    qDeleteAll(current_crew_turn_items);
    current_crew_turn_items.clear();

    // Ensure the entire turn log is clean
    while (ui->turn_log->topLevelItemCount() > 2) {
        delete ui->turn_log->takeTopLevelItem(0);
    }
    ui->turn_log->setEnabled(false);
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
    int fuel_used = maneuver_scene->getSelectedManeuver().right(1).toInt();

    // First turn should assume that the altitude is what the plane has been at already, thus just return the speed
    if (ui->turn_log->topLevelItemCount() < 3) {
        return fuel_used;
    }

    int alt = alt_cmb->currentText().toInt();
    int last_alt = ui->turn_log->topLevelItem(ui->turn_log->topLevelItemCount()-3)->text(Altitude).toInt();
    int last_last_alt = last_alt;
    if (ui->turn_log->topLevelItemCount() > 3) {
        ui->turn_log->topLevelItem(ui->turn_log->topLevelItemCount()-4)->text(Altitude).toInt();
    }

    // Check if the current altitude is higher/lower than the previous turn's altitude
    bool climbing = alt > last_alt;
    bool diving = alt < last_alt;

    // Check for zoom climbing
    int prev_delta = 0;
    if (ui->turn_log->topLevelItemCount() > 2) {
        prev_delta = last_alt - last_last_alt;
    }
    bool zoom_climbing = prev_delta <= -2 && climbing;

    // If climbing, add one fuel unless zoom climbing
    // If diving, subtract fuel used to a minimum of 0
    if (climbing) {
        if (zoom_climbing) {
            return fuel_used;
        }
        return fuel_used + 1;
    }
    else if (diving) {
        if (last_alt - alt >= 0) {
            return fuel_used - (last_alt - alt);
        }
        return 0;
    }
    return fuel_used;
}

void MainWindow::setAvailableAltitudes()
{
    QPersistentModelIndex current_maneuver = maneuver_scene->getSelectedManeuverIdx();
    alt_cmb->setDisabled(current_maneuver == QPersistentModelIndex());
    if (current_maneuver == QPersistentModelIndex()) {
        return;
    }

    // If it's the start of the game, don't restrict altitudes
    if (ui->turn_log->topLevelItemCount() < 3) {
        return;
    }

    auto * model = qobject_cast<QStandardItemModel*>(alt_cmb->model());
    if(!model) {
        return;
    }

    // Cache common values
    QString maneuver_dive_val = current_maneuver.sibling(current_maneuver.row(), ManeuverItem::Dive_Value).data().toString();
    QString maneuver_climb_val = current_maneuver.sibling(current_maneuver.row(), ManeuverItem::Climb_Value).data().toString();
    auto plane_idx = plane_action_group->checkedAction()->data().toPersistentModelIndex();
    int current_alt = ui->turn_log->topLevelItem(ui->turn_log->topLevelItemCount()-3)->text(Altitude).toInt();

    // Determine the minimum altitude the player can dive to
    int min = maneuver_dive_val == "D1" ? current_alt - 1 : maneuver_dive_val == "-" ? current_alt : 1;

    // Determine if the player can climb
    int can_climb_to = plane_idx.sibling(plane_idx.row(), PlaneItem::Rated_Climb).data().toInt() + current_alt;
    int max_alt = plane_idx.sibling(plane_idx.row(), PlaneItem::Max_Altitude).data().toInt();

    if (!plane_idx.sibling(plane_idx.row(), PlaneItem::Can_Return_To_Max_Alt).data().toBool()) {
        max_alt--;
    }
    if (maneuver_climb_val == "C1") {
        max_alt = std::min(current_alt + 1, max_alt);
    }
    else if (maneuver_climb_val == "-") {
        max_alt = current_alt;
    }
    else if (maneuver_climb_val == "C") {
        max_alt = std::min(max_alt, can_climb_to);
    }

    for (int i=0; i<alt_cmb->count(); ++i) {
        auto * item = model->item(i);
        if(!item) {
            continue;
        }
        item->setEnabled(item->text().toInt() >= min && item->text().toInt() <= max_alt);
    }
}

void MainWindow::setAvailableManeuvers()
{
    auto plane_idx = plane_action_group->checkedAction()->data().toPersistentModelIndex();
    auto pilot_ability_idx = crew_proxy_model->index(0, CrewItem::Crew_Ability,  crew_proxy_model->mapFromSource(plane_idx));
    for (int i=0; i<maneuver_proxy_model->rowCount(maneuver_proxy_model->mapFromSource(plane_idx)); ++i) {
        // Get the iterated index that we'll be checking against
        QModelIndex maneuver_idx = maneuver_proxy_model->index(i, ManeuverItem::Can_Be_Used, maneuver_proxy_model->mapFromSource(plane_idx));
        QString maneuver_direction = maneuver_idx.sibling(i, ManeuverItem::Direction).data().toString();
        // First turn should be a straight maneuver
        if (ui->turn_log->topLevelItemCount() < 3) {
            if (maneuver_idx.sibling(i, ManeuverItem::Is_Restricted).data().toBool() &&
                    pilot_ability_idx.data().toInt() != CrewItem::Unrestricted_Maneuvers) {
                maneuver_proxy_model->setData(maneuver_idx, false);
            }
            else {
                maneuver_proxy_model->setData(maneuver_idx, true);
            }
        }
        else {
            // On other turns, start by filtering out any maneuver that would be out of reach of the plane due to stability
            QPersistentModelIndex selected_maneuver = maneuver_scene->getSelectedManeuverIdx();
            QString last_direction = selected_maneuver.sibling(selected_maneuver.row(), ManeuverItem::Direction).data().toString();
            // Add L, S, and R depending on what directions are allowed. Can be combined
            QString available_directions;
            if (maneuver_proxy_model->index(0, PlaneItem::Stability).data().toString() == "C") {
                available_directions = "LSR";
            }
            else if (last_direction == "L") {
                available_directions = "LS";
            }
            else if (last_direction == "S") {
                available_directions = "LSR";
            }
            else if (last_direction == "R") {
                available_directions = "SR";
            }
            // Additional possible constraints for restricted maneuvers
            if (maneuver_idx.sibling(i, ManeuverItem::Is_Restricted).data().toBool() &&
                    selected_maneuver.data().toString() != "2S1" && selected_maneuver.data().toString() != "3S2" &&
                    selected_maneuver.data().toString() != "4S3" && selected_maneuver.data().toString() != "5S4" &&
                    pilot_ability_idx.data().toInt() != CrewItem::Unrestricted_Maneuvers) {
                maneuver_proxy_model->setData(maneuver_idx, false);
            }
            // If the maneuver isn't allowed to be selected based on turn direction, exclude it here and move on
            else if (!available_directions.contains(maneuver_direction)){
                maneuver_proxy_model->setData(maneuver_idx, false);
            }
            else {
                // Next check speed
                int last_speed = selected_maneuver.sibling(selected_maneuver.row(), ManeuverItem::Speed).data().toInt();
                int maneuver_speed = maneuver_idx.sibling(i, ManeuverItem::Speed).data().toInt();
                maneuver_proxy_model->setData(maneuver_idx, last_speed - 1 <= maneuver_speed && maneuver_speed <= last_speed + 1);
            }
        }
        maneuver_scene->updateManeuver(maneuver_idx.sibling(i, ManeuverItem::Maneuver_Name).data().toString());
    }
    maneuver_scene->update();
}
