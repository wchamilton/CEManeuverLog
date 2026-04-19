#include "TurnTrackerDialog.h"
#include "ui_TurnTrackerDialog.h"

#include "CEManeuvers.h"
#include "CrewControls.h"
#include "GunRotationControl.h"
#include "StatusEffectsInterface.h"
#include "models/GameModel.h"
#include "models/GameModelItems.h"
#include "graphics/ManeuverScene.h"
#include "graphics/AltCtrlScene.h"
#include "TurnHistory/TurnHistoryDialog.h"

TurnTrackerDialog::TurnTrackerDialog(GameModel* game_model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TurnTrackerDialog),
    game_model(game_model)
{
    ui->setupUi(this);

    // Initialize proxy models
    maneuver_proxy = QSharedPointer<FilterProxy>::create(game_model, this);
    maneuver_proxy->setTypeFilter(BaseItem::Plane_Maneuver_Item_Type);
    maneuver_proxy->setRecursiveFilteringEnabled(true);

    crew_proxy = QSharedPointer<FilterProxy>::create(game_model, this);
    crew_proxy->setTypeFilter(BaseItem::Plane_Crew_Item_Type);
    crew_proxy->setRecursiveFilteringEnabled(true);
    crew_proxy->setAutoAcceptChildRows(true);

    turn_proxy = QSharedPointer<FilterProxy>::create(game_model, this);
    turn_proxy->setTypeFilter(BaseItem::Turn_Item_Type);
    turn_proxy->setRecursiveFilteringEnabled(true);
    turn_proxy->setAutoAcceptChildRows(true);

    effects_proxy = QSharedPointer<FilterProxy>::create(game_model, this);
    effects_proxy->setTypeFilter({BaseItem::Plane_Item_Type, BaseItem::Plane_Effect_Item_Type});
    effects_proxy->setRecursiveFilteringEnabled(true);
    turn_proxy->setAutoAcceptChildRows(true);

    // Extract the selected plane index
    QModelIndex game_idx = game_model->gameRootIdx();
    plane_idx = game_idx.sibling(game_idx.row(), GameItem::Game_Plane_Selected).data().toPersistentModelIndex();

    ui->alt_ctrl_grp_box->setTitle(plane_idx.data().toString());

    // Set up the damage trackers
    ui->engine_grp->setTitle("Engine");
    ui->wing_grp->setTitle("Wing");
    ui->fuselage_grp->setTitle("Fuselage");
    ui->tail_grp->setTitle("Tail");

    ui->engine_grp->setHPValues(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Engine_HP).data().toInt(),
                                plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Engine_Critical).data().toInt());
    ui->wing_grp->setHPValues(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Wing_HP).data().toInt(),
                                plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Wing_Critical).data().toInt());
    ui->fuselage_grp->setHPValues(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Fuselage_HP).data().toInt(),
                                plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Fuselage_Critical).data().toInt());
    ui->tail_grp->setHPValues(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Tail_HP).data().toInt(),
                                plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Tail_Critical).data().toInt());

    // Init and assign the graphics scenes
    maneuver_scene = new ManeuverScene(maneuver_proxy.data(), maneuver_proxy->mapFromSource(plane_idx), ui->maneuver_gv);
    ui->maneuver_gv->setScene(maneuver_scene);

    alt_ctrl_scene = new AltCtrlScene(plane_idx, ui->alt_ctrl_gv);
    ui->alt_ctrl_gv->setScene(alt_ctrl_scene);

    // Iterate over the crew members and populate their controls
    QModelIndex plane_filtered_idx = crew_proxy->mapFromSource(plane_idx);
    for (int crew_row=0; crew_row<crew_proxy->rowCount(plane_filtered_idx); ++crew_row) {
        QModelIndex crew_idx = crew_proxy->index(crew_row, PlaneCrewItem::Plane_Crew_Name, plane_filtered_idx);
        CrewControls* cc = new CrewControls(crew_idx, crew_proxy, maneuver_proxy, ui->crew_tab);
        ui->crew_tab->addTab(cc, crew_idx.sibling(crew_idx.row(), PlaneCrewItem::Plane_Crew_Role).data().toString() + " (" + crew_idx.data().toString() + ")");

        // If the current crew member is also the pilot, store that so we can later check for movement capabilities/restrictions
        if (crew_idx.sibling(crew_idx.row(), PlaneCrewItem::Plane_Crew_Role_ID).data().toInt() == PlaneCrewItem::Pilot) {
            pilot_idx = crew_idx;
        }

        connect(maneuver_scene, &ManeuverScene::maneuverClicked, cc, &CrewControls::applyManeuverRestrictions);

        // Add each of the guns to the firing arc combobox
        for (int gun_row=0; gun_row<crew_proxy->rowCount(crew_idx); ++gun_row) {
            QModelIndex gun_idx = crew_proxy->index(gun_row, PlaneArmamentsItem::Plane_Armaments_Name, crew_idx);
            ui->gun_rotation_tab->addTab(new GunRotationControl(crew_proxy, gun_idx, ui->gun_rotation_tab), QString("%1 (%2)").arg(gun_idx.data().toString(), crew_idx.data().toString()));
        }
    }

    turn_log_view = new TurnHistoryDialog(game_model, crew_proxy, this);
    connect(ui->view_log_btn, &QPushButton::clicked, turn_log_view, &QDialog::show);

    effects_interface = new StatusEffectsInterface(crew_proxy, effects_proxy, plane_idx, ui->status_effects_grpbox);
    ui->status_effects_grpbox->layout()->addWidget(effects_interface);
    connect(maneuver_scene, &ManeuverScene::maneuverClicked, this, &TurnTrackerDialog::handleManeuverSelection);
    connect(ui->end_turn_btn, &QPushButton::clicked, this, &TurnTrackerDialog::handleTurnEnd);

    // Set the initial enabled/disabled state of the maneuvers
    updateAvailableManeuvers();
}

TurnTrackerDialog::~TurnTrackerDialog()
{
    delete ui;
    delete turn_log_view;
}

void TurnTrackerDialog::handleTurnEnd()
{
    // Iterate over the crew and prepare the turn cache
    for (int i=0; i<ui->crew_tab->count(); ++i) {
        // Trigger widget refreshes for each of the crew
        if (auto tab = dynamic_cast<CrewControls*>(ui->crew_tab->widget(i))) {
            tab->saveCrewData();
            tab->resetSelection();
        }
    }
    for (int i=0; i<ui->gun_rotation_tab->count(); ++i) {
        // Trigger widget refreshes for each of the crew
        if (auto tab = dynamic_cast<GunRotationControl*>(ui->gun_rotation_tab->widget(i))) {
            tab->saveGunRotation();
        }        
    }

    // Calculate and update current fuel reserves. Do this before creating a new turn index
    QModelIndex plane_fuel_idx = plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Current_Fuel);
    int fuel_consumed = calculateFuelUsage();
    int fuel_reserves = plane_fuel_idx.data().toInt() - fuel_consumed;
    game_model->setData(plane_fuel_idx, fuel_reserves);

    // Create the top level turn index and cache the values for the turn
    QModelIndex turn_idx = game_model->addTurn();
    BaseItem* turn_idx_ptr = static_cast<BaseItem*>(turn_idx.internalPointer());
    BaseItem* plane_idx_ptr = static_cast<BaseItem*>(plane_idx.internalPointer());
    QModelIndex maneuver_idx = maneuver_scene->getSelectedManeuverIdx();

    // First update the newly created turn item to cache the state of the various components
    turn_idx_ptr->setData(TurnItem::Turn_Number, QString("Turn %1").arg(game_model->rowCount(game_model->turnsRootIdx())));
    turn_idx_ptr->setData(TurnItem::Turn_Selected_Maneuver_Idx, QPersistentModelIndex(maneuver_idx));
    turn_idx_ptr->setData(TurnItem::Turn_Plane_Alt, alt_ctrl_scene->getCurrentAlt());
    turn_idx_ptr->setData(TurnItem::Turn_Plane_Speed_Last_Turn, plane_idx_ptr->data(PlaneItem::Plane_Current_Speed));
    turn_idx_ptr->setData(TurnItem::Turn_Plane_Alt_Last_Turn, plane_idx_ptr->data(PlaneItem::Plane_Current_Alt));
    turn_idx_ptr->setData(TurnItem::Turn_Plane_Fuel_Used, fuel_consumed);
    turn_idx_ptr->setData(TurnItem::Turn_Plane_Fuel, fuel_reserves);
    turn_idx_ptr->setData(TurnItem::Turn_Plane_Engine_HP, plane_idx_ptr->data(PlaneItem::Plane_Engine_HP));
    turn_idx_ptr->setData(TurnItem::Turn_Plane_Wing_HP, plane_idx_ptr->data(PlaneItem::Plane_Wing_HP));
    turn_idx_ptr->setData(TurnItem::Turn_Plane_Fuse_HP, plane_idx_ptr->data(PlaneItem::Plane_Fuselage_HP));
    turn_idx_ptr->setData(TurnItem::Turn_Plane_Tail_HP, plane_idx_ptr->data(PlaneItem::Plane_Tail_HP));
    turn_idx_ptr->setData(TurnItem::Turn_Plane_Payload_Count, plane_idx_ptr->data(PlaneItem::Plane_Payload_Count));
    game_model->dataChanged(turn_idx.sibling(turn_idx.row(), 0), turn_idx.sibling(turn_idx.row(), TurnItem::Turn_Plane_Payload_Count));

    // Create the turn plane effect items
    QModelIndex plane_effects_filtered_idx = effects_proxy->mapFromSource(plane_idx);
    QList<QPersistentModelIndex> effects_to_remove;

    for (int effects_row=0; effects_row<effects_proxy->rowCount(plane_effects_filtered_idx); ++effects_row) {
        QModelIndex plane_effects_idx = effects_proxy->index(effects_row, PlaneEffectItem::Plane_Effect_ID, plane_effects_filtered_idx);
        QModelIndex turn_effects_idx = game_model->addTurnPlaneEffects(turn_idx);
        BaseItem* plane_effects_ptr = static_cast<BaseItem*>(effects_proxy->mapToSource(plane_effects_idx).internalPointer());
        BaseItem* turn_effects_idx_ptr = static_cast<BaseItem*>(turn_effects_idx.internalPointer());

        turn_effects_idx_ptr->setData(TurnPlaneEffects::Turn_Plane_Effect_ID, plane_effects_idx.data());
        turn_effects_idx_ptr->setData(TurnPlaneEffects::Turn_Plane_Effect_Name, plane_effects_ptr->data(PlaneEffectItem::Plane_Effect_Name));
        turn_effects_idx_ptr->setData(TurnPlaneEffects::Turn_Plane_Effect_Desc, plane_effects_ptr->data(PlaneEffectItem::Plane_Effect_Desc));
        int remaining_turns = plane_effects_ptr->data(PlaneEffectItem::Plane_Effect_Remaining_Turns).toInt();
        turn_effects_idx_ptr->setData(TurnPlaneEffects::Turn_Plane_Effect_Remaining_Turns, remaining_turns);
        game_model->dataChanged(turn_effects_idx.sibling(turn_effects_idx.row(), 0), turn_effects_idx.sibling(turn_effects_idx.row(), TurnPlaneEffects::Turn_Plane_Effect_Remaining_Turns));

        if (remaining_turns > 0) {
            effects_proxy->setData(plane_effects_idx.sibling(plane_effects_idx.row(), PlaneEffectItem::Plane_Effect_Remaining_Turns), --remaining_turns);
            if (remaining_turns == 0) {
                effects_to_remove << effects_proxy->mapToSource(plane_effects_idx);
            }
        }
    }
    for (const auto &idx : std::as_const(effects_to_remove)) {
        game_model->removeEffect(idx);
    }

    // Iterate over the crew and their guns to create and populate their turn items
    QModelIndex plane_crew_filtered_idx = crew_proxy->mapFromSource(plane_idx);
    for (int crew_row=0; crew_row<crew_proxy->rowCount(plane_crew_filtered_idx); ++crew_row) {
        QModelIndex crew_idx = crew_proxy->index(crew_row, PlaneCrewItem::Plane_Crew_Name, plane_crew_filtered_idx);
        QModelIndex turn_crew_idx = game_model->addTurnCrew(turn_idx);
        BaseItem* crew_idx_ptr = static_cast<BaseItem*>(crew_proxy->mapToSource(crew_idx).internalPointer());
        BaseItem* turn_crew_idx_ptr = static_cast<BaseItem*>(turn_crew_idx.internalPointer());

        turn_crew_idx_ptr->setData(TurnCrewItem::Turn_Crew_Idx, QPersistentModelIndex(crew_proxy->mapToSource(crew_idx)));
        turn_crew_idx_ptr->setData(TurnCrewItem::Turn_Crew_Action_Taken, crew_idx_ptr->data(PlaneCrewItem::Plane_Crew_Action_Taken));
        turn_crew_idx_ptr->setData(TurnCrewItem::Turn_Crew_Action_Extra_Data, crew_idx_ptr->data(PlaneCrewItem::Plane_Crew_Action_Extra_Data));
        turn_crew_idx_ptr->setData(TurnCrewItem::Turn_Crew_Wounds_Accrued, crew_idx_ptr->data(PlaneCrewItem::Plane_Crew_Wounds));
        turn_crew_idx_ptr->setData(TurnCrewItem::Turn_Crew_Total_Red_Hits, crew_idx_ptr->data(PlaneCrewItem::Plane_Crew_Reds));
        turn_crew_idx_ptr->setData(TurnCrewItem::Turn_Crew_Total_Kills, crew_idx_ptr->data(PlaneCrewItem::Plane_Crew_Kills));
        game_model->dataChanged(turn_crew_idx.sibling(turn_crew_idx.row(), 0), turn_crew_idx.sibling(turn_crew_idx.row(), TurnCrewItem::Turn_Crew_Total_Kills));

        for (int gun_row=0; gun_row<crew_proxy->rowCount(crew_idx); ++gun_row) {
            QModelIndex gun_idx = crew_proxy->index(gun_row, PlaneArmamentsItem::Plane_Armaments_Name, crew_idx);
            QModelIndex turn_gun_idx = game_model->addTurnCrewGun(turn_crew_idx);
            BaseItem* gun_idx_ptr = static_cast<BaseItem*>(crew_proxy->mapToSource(gun_idx).internalPointer());
            BaseItem* turn_gun_idx_ptr = static_cast<BaseItem*>(turn_gun_idx.internalPointer());

            turn_gun_idx_ptr->setData(TurnArmamentItem::Turn_Crew_Armament_Idx, QPersistentModelIndex(crew_proxy->mapToSource(gun_idx)));
            turn_gun_idx_ptr->setData(TurnArmamentItem::Turn_Crew_Armament_Position, gun_idx_ptr->data(PlaneArmamentsItem::Plane_Armaments_Gun_Rotation_Last_Turn));
            turn_gun_idx_ptr->setData(TurnArmamentItem::Turn_Crew_Armament_Is_Destroyed, gun_idx_ptr->data(PlaneArmamentsItem::Plane_Armaments_Gun_Destroyed));
            turn_gun_idx_ptr->setData(TurnArmamentItem::Turn_Crew_Armament_IsJammed, gun_idx_ptr->data(PlaneArmamentsItem::Plane_Armaments_Gun_Jammed));
            turn_gun_idx_ptr->setData(TurnArmamentItem::Turn_Crew_Armament_Current_Box_Ammo, gun_idx_ptr->data(PlaneArmamentsItem::Plane_Armaments_Ammo_In_Current_Box));
            turn_gun_idx_ptr->setData(TurnArmamentItem::Turn_Crew_Armament_Remaining_Ammo_Boxes, gun_idx_ptr->data(PlaneArmamentsItem::Plane_Armaments_Spare_Ammo_Box_Count));
            game_model->dataChanged(turn_gun_idx.sibling(turn_gun_idx.row(), 0), turn_gun_idx.sibling(turn_gun_idx.row(), TurnArmamentItem::Turn_Crew_Armament_Remaining_Ammo_Boxes));
        }
    }

    ui->lock_movement_btn->setChecked(false);
    ui->lock_movement_btn->setEnabled(false);
    game_model->writeTurnsToJSON();
    ui->view_log_btn->setEnabled(true);

    // Update components that change dynamically at end-of-turn to reflect user selections
    game_model->setData(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Current_Alt), alt_ctrl_scene->getCurrentAlt());
    game_model->setData(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Current_Speed), maneuver_idx.siblingAtColumn(PlaneManeuverItem::Plane_Maneuver_Speed).data());

    maneuver_scene->clearSelection();
    updateAvailableManeuvers();
    effects_interface->refreshEffectsList();

    // We only create turn entries at end-of-turn so we want to display one more than the count
    alt_ctrl_scene->updateTurnCounter(game_model->rowCount(game_model->turnsRootIdx()) + 1);
    alt_ctrl_scene->setManeuver(QModelIndex());
}

void TurnTrackerDialog::handleManeuverSelection(QModelIndex maneuver_idx)
{
    int current_alt = plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Current_Alt).data().toInt();
    int climb_range = plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Rated_Climb).data().toInt();
    int dive_range = plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Rated_Dive).data().toInt();
    QString maneuver_climb_val = maneuver_idx.sibling(maneuver_idx.row(), PlaneManeuverItem::Plane_Maneuver_Climb_Val).data().toString();
    QString maneuver_dive_val = maneuver_idx.sibling(maneuver_idx.row(), PlaneManeuverItem::Plane_Maneuver_Dive_Val).data().toString();
    if (maneuver_climb_val == "-") {
        climb_range = 0;
    }
    else if (maneuver_climb_val.right(1) == "1") {
        climb_range = 1;
    }
    if (maneuver_dive_val == "-") {
        dive_range = 0;
    }
    else if (maneuver_dive_val.right(1) == "1") {
        dive_range = 1;
    }

    int lowest = std::max(0, current_alt-dive_range);
    int highest = current_alt+climb_range;
    QList<int> alts(highest - lowest + 1);
    std::iota(alts.begin(), alts.end(), lowest);
    alt_ctrl_scene->updateAltitudes(alts, current_alt);
    alt_ctrl_scene->setManeuver(maneuver_idx);
    ui->lock_movement_btn->setEnabled(true);
}

int TurnTrackerDialog::calculateFuelUsage()
{
    QModelIndex selected_maneuver_idx = maneuver_scene->getSelectedManeuverIdx();

    // Cache the altitudes to determine if changes in altitude impact fuel use
    int alt_last_turn = game_model->currentTurn(TurnItem::Turn_Plane_Alt_Last_Turn).data().toInt();
    int current_alt = plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Current_Alt).data().toInt();
    int selected_next_alt = alt_ctrl_scene->getCurrentAlt();

    // Determine which kind of altitude change there is, if any
    bool diving = selected_next_alt < current_alt;
    bool climbing = selected_next_alt > current_alt;

    // If there was a dive of two or more last turn and this turn we're climbing, that's a zoom climb
    bool zoom_climbing = (alt_last_turn-current_alt >= 2) && climbing;

    // Extract the maneuver's speed as that's the initial starting point for fuel usage
    int fuel_used = selected_maneuver_idx.sibling(selected_maneuver_idx.row(), PlaneManeuverItem::Plane_Maneuver_Speed).data().toInt();

    if (diving) {
        --fuel_used;
    }
    else if (climbing && !zoom_climbing) {
        ++fuel_used;
    }

    return fuel_used;
}

void TurnTrackerDialog::updateAvailableManeuvers()
{
    int prev_alt = plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Current_Alt).data().toInt();
    int forced_alt_delta = prev_alt == 0 ? 1 : 0; // Check for 0 here in case this is a scramble mission where starting alt is 0
    QChar prev_direction = 'S';
    QChar rudder_jam_direction;
    bool restricted_maneuvers_allowed = false;
    int forced_speed = false;

    // Apply start-of-game restrictions
    if (game_model->rowCount(game_model->turnsRootIdx()) == 0) {
        forced_speed = true;
    }
    // Overwrite previous defaults with restrictions based on prior turns
    else {
        QModelIndex prev_maneuver_idx = game_model->currentTurn(TurnItem::Turn_Selected_Maneuver_Idx).data().toModelIndex();
        prev_direction = prev_maneuver_idx.sibling(prev_maneuver_idx.row(), PlaneManeuverItem::Plane_Maneuver_Direction).data().toChar();
        QString maneuver_name = prev_maneuver_idx.sibling(prev_maneuver_idx.row(), PlaneManeuverItem::Plane_Maneuver_Name).data().toString();
        if (maneuver_name == "2S1" || maneuver_name == "3S2" || maneuver_name == "4S3" || maneuver_name == "5S4") {
            restricted_maneuvers_allowed = true;
        }

        for (int i=0; i<game_model->rowCount(game_model->currentTurn()); ++i) {
            QModelIndex child = game_model->index(i, 0, game_model->currentTurn());
            if (child.data(Qt::UserRole).toInt() == BaseItem::Turn_Effects_Type) {
                if (child.sibling(child.row(), TurnPlaneEffects::Turn_Plane_Effect_ID).data().toInt() == PlaneEffectItem::Effect_Rudder_Jam_Left) {
                    rudder_jam_direction = 'L';
                }
                else if (child.sibling(child.row(), TurnPlaneEffects::Turn_Plane_Effect_ID).data().toInt() == PlaneEffectItem::Effect_Rudder_Jam_Right) {
                    rudder_jam_direction = 'R';
                }
            }
            if (child.data(Qt::UserRole).toInt() == BaseItem::Turn_Crew_Item_Type) {
                if (child.sibling(child.row(), TurnCrewItem::Turn_Crew_Action_Taken).data().toInt() == PlaneCrewItem::Action_Shoot) {
                    PlaneCrewItem::ShotProperties shot_props = child.sibling(child.row(), TurnCrewItem::Turn_Crew_Action_Extra_Data).data().value<PlaneCrewItem::ShotProperties>();
                    forced_alt_delta = shot_props.target_range < 3 ? shot_props.target_delta : 0;
                }
            }
        }
    }

    maneuver_scene->setManeuversAvailable(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Current_Speed).data().toInt(),
                                          prev_direction,
                                          prev_alt,
                                          forced_alt_delta,
                                          rudder_jam_direction,
                                          restricted_maneuvers_allowed,
                                          pilot_idx.sibling(pilot_idx.row(), PlaneCrewItem::Plane_Crew_Ability_Unrestricted_Maneuvers).data().toBool(),
                                          forced_speed);
}
