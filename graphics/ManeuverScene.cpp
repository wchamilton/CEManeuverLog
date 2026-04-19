#include "ManeuverScene.h"
#include "ManeuverGraphic.h"
#include "CEManeuvers.h"
#include "models/GameModelItems.h"

//#include <QtSvg/QGraphicsSvgItem>
#include <QPersistentModelIndex>

ManeuverScene::ManeuverScene(QAbstractItemModel* model, QPersistentModelIndex filtered_plane_idx, QObject *parent) :
    QGraphicsScene(parent)
{
    applyScheduleBG();

    // Add the maneuvers for the plane
    for (int i=0; i<model->rowCount(filtered_plane_idx); ++i) {
        QPersistentModelIndex maneuver_idx = model->index(i, PlaneManeuverItem::Plane_Maneuver_Name, filtered_plane_idx);
        addManeuver(maneuver_idx);
    }

    connect(this, &ManeuverScene::focusItemChanged, this, &ManeuverScene::handleFocusChanges);
}

ManeuverGraphic *ManeuverScene::getManeuver(QString maneuver_name)
{
    return maneuver_map[maneuver_name];
}

QPersistentModelIndex ManeuverScene::getSelectedManeuverIdx()
{
    return selected_maneuver ? selected_maneuver->getIdx() : QPersistentModelIndex();
}

QString ManeuverScene::getSelectedManeuver()
{
    return selected_maneuver ? maneuver_map.key(selected_maneuver) : QString();
}

void ManeuverScene::addManeuver(QPersistentModelIndex maneuver_idx)
{
    Maneuver maneuver = master_maneuver_map[maneuver_idx.data().toString()];
    ManeuverGraphic::ShiftText shift_val = ManeuverGraphic::Shift_None;
    if (maneuver.name == "1L0" || maneuver.name == "9L2" || maneuver.name == "11L2" || maneuver.name == "28R1") {
        shift_val = ManeuverGraphic::Shift_Left;
    }
    else if (maneuver.name == "1S0" || maneuver.name == "1R0"  || maneuver.name == "0S1" ||
             maneuver.name == "9R2" || maneuver.name == "11R2" || maneuver.name == "28L1" || maneuver.name == "27S2") {
        shift_val = ManeuverGraphic::Shift_Right;
    }
    maneuver_map[maneuver.name] = new ManeuverGraphic(maneuver_idx, shift_val);
    maneuver_map[maneuver.name]->setPos(maneuver.pos);
    for (int i=0; i<maneuver.tile_movements.size()-1; ++i) {
        maneuver_map[maneuver.name]->addHex(maneuver.tile_movements.at(i));
    }
    maneuver_map[maneuver.name]->addHex(maneuver.tile_movements.last(),
                                        maneuver.name == "0S1" ? HexTile::Spin_Tile : HexTile::Plane_Icon_Tile,
                                        maneuver.final_rotation);
    addItem(maneuver_map[maneuver.name]);
}

void ManeuverScene::removeManeuver(QPersistentModelIndex maneuver_idx)
{
    if (maneuver_map.contains(maneuver_idx.data().toString())) {
        ManeuverGraphic* maneuver_item = maneuver_map.take(maneuver_idx.data().toString());
        removeItem(maneuver_item);
        delete maneuver_item;
    }
}

void ManeuverScene::clearSelection()
{
    setFocusItem(background_item);
    if (selected_maneuver) {
        selected_maneuver->setSelected(false);
        selected_maneuver->update();
        selected_maneuver = nullptr;
    }
    QGraphicsScene::clearSelection();
}

void ManeuverScene::applyScheduleBG()
{
    // Add the background as a pixmap
    //    QGraphicsSvgItem* background_item = new QGraphicsSvgItem(QString("../CEManeuverLog/graphics/Canvas_AircraftSheet.svg"));
    //    background_item->setZValue(-1);
    //    addItem(background_item);
    background_item = addPixmap(QPixmap(GRAPHICS_LOCATION + "/background.png"));
    background_item->setFlag(QGraphicsItem::ItemIsFocusable);
    background_item->setZValue(-1);
}

void ManeuverScene::handleFocusChanges(QGraphicsItem *newFocusItem, QGraphicsItem *oldFocusItem, Qt::FocusReason reason)
{
    Q_UNUSED(oldFocusItem)
    Q_UNUSED(reason)

    ManeuverGraphic* new_maneuver = dynamic_cast<ManeuverGraphic*>(newFocusItem);
    ManeuverGraphic* old_maneuver = selected_maneuver;
    if (newFocusItem == background_item){
        return;
    }
    if (new_maneuver != nullptr) {
        if (old_maneuver != nullptr) {
            old_maneuver->setSelected(false);
        }
        new_maneuver->setSelected(true);
        selected_maneuver = new_maneuver;
        emit maneuverClicked(new_maneuver->getIdx());
    }
    update();
}

void ManeuverScene::setManeuversAvailable(int prev_speed, QChar prev_direction, int prev_alt, int forced_alt_delta, QChar rudder_jam_direction, bool restricted_maneuvers_allowed, bool has_unrestricted_maneuvers, bool forced_speed)
{
    for (auto [maneuver_name, maneuver] : maneuver_map.asKeyValueRange()) {
        QModelIndex maneuver_idx = maneuver->getIdx();
        QModelIndex plane_idx = maneuver_idx.parent();
        maneuver->setEnabled(true); // Initially assume all maneuvers are allowed and they'll be restricted later

        // Always allow the spin maneuver and stall to be selected for reasons
        if (maneuver_name == "0S1" || maneuver_name == "1L0" || maneuver_name == "1S0" || maneuver_name == "1R0") {
            continue;
        }

        int payload_count = plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Payload_Count).data().toInt();
        bool is_weight_restricted = maneuver_idx.sibling(maneuver_idx.row(), PlaneManeuverItem::Plane_Maneuver_Has_Weight_Restriction).data().toBool();
        if (is_weight_restricted && payload_count > 0) {
            maneuver->setEnabled(false);
            continue;
        }

        int maneuver_speed = maneuver_idx.sibling(maneuver_idx.row(), PlaneManeuverItem::Plane_Maneuver_Speed).data().toInt();
        // If the prev_speed is forced like if it's the start of game then exclude any maneuvers that are not that speed
        if (forced_speed && maneuver_speed != prev_speed) {
            maneuver->setEnabled(false);
            continue;
        }
        // Other speed changes can only be +/- 1
        if (std::abs(prev_speed - maneuver_speed) > 1) {
            maneuver->setEnabled(false);
            continue;
        }

        // If the maneuver has a weight restriction and the plane still has bombs then disable the maneuver
        // For maneuvers that are "weight restricted" by additional guns, there's no way to lose said weight so they're just created as a different plane file
        if (maneuver_idx.sibling(maneuver_idx.row(), PlaneManeuverItem::Plane_Maneuver_Has_Weight_Restriction).data().toBool() &&
            plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Payload_Count).data().toInt() > 0) {
            maneuver->setEnabled(false);
            continue;
        }

        // Based on last maneuver, apply restrictions. If this is the first turn, it's assumed a maneuver with S was used to start
        // Directional restrictions. S can go either direction; L or R can only go in their direction or S. Rudder jam forces a set direction
        QChar maneuver_direction = maneuver_idx.sibling(maneuver_idx.row(), PlaneManeuverItem::Plane_Maneuver_Direction).data().toChar();
        if (rudder_jam_direction != QChar()) {
            if (maneuver_direction == 'S') {
                maneuver->setEnabled(false);
                continue;
            }
            if (((rudder_jam_direction == 'R') && maneuver_direction == 'L') ||
                ((rudder_jam_direction == 'L') && maneuver_direction == 'R')) {
                maneuver->setEnabled(false);
                continue;
            }
        }
        // We check prev_direction separately as a plane with stability C doesn't follow the same rules when it comes to flipping between L and R
        else if (plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Stability_Rating).data().toChar() != 'C') {
            if (((prev_direction == 'R') && maneuver_direction == 'L') ||
                ((prev_direction == 'L') && maneuver_direction == 'R')) {
                maneuver->setEnabled(false);
                continue;
            }
        }

        // Check altitude constraints. This may involve a forced direction due to a shot at a target which is depicted by forced_shot_delta
        // If forced_shot_delta == -1 then only maneuvers that allow a dive are allowed
        QString dive_tolerance = maneuver_idx.sibling(maneuver_idx.row(), PlaneManeuverItem::Plane_Maneuver_Dive_Val).data().toString();
        if (forced_alt_delta == -1 && dive_tolerance != "D1" && dive_tolerance != "D") {
            maneuver->setEnabled(false);
            continue;
        }
        // If forced_shot_delta == 1 then only maneuvers that allow a climb are allowed
        QString climb_tolerance = maneuver_idx.sibling(maneuver_idx.row(), PlaneManeuverItem::Plane_Maneuver_Climb_Val).data().toString();
        if (forced_alt_delta == 1 && climb_tolerance != "C1" && climb_tolerance != "C") {
            maneuver->setEnabled(false);
            continue;
        }
        // Prevent dive-only maneuvers when at min alt
        QString level_tolerance = maneuver_idx.sibling(maneuver_idx.row(), PlaneManeuverItem::Plane_Maneuver_Level_Val).data().toString();
        if (prev_alt == 1 && (dive_tolerance == "D1" || dive_tolerance == "D") && climb_tolerance == "-" && level_tolerance == "-") {
            maneuver->setEnabled(false);
            continue;
        }

        // Don't allow restricted maneuvers unless the prior movement was one that allowed pilot reloads OR the pilot has unrestricted maneuvers
        // The maneuvers must still respect the direction and speed constraints which should have been done above
        bool maneuver_is_restricted = maneuver_idx.sibling(maneuver_idx.row(), PlaneManeuverItem::Plane_Maneuver_Is_Restricted).data().toBool();
        if (maneuver_is_restricted && !restricted_maneuvers_allowed && !has_unrestricted_maneuvers) {
            maneuver->setEnabled(false);
            continue;
        }
    }

    // QModelIndex last_turn = turn_model->lastIndex(TurnItem::Turn_Maneuver_Col);
    // QPersistentModelIndex last_maneuver = last_turn.isValid() ? last_turn.data(Qt::UserRole).toPersistentModelIndex() : QPersistentModelIndex();
    // int prev_alt = last_turn.isValid() ? last_turn.sibling(last_turn.row(), TurnItem::Turn_Altitude_Col).data(Qt::UserRole).toInt() : turn_model->getStartingAlt();
    // int prev_speed = last_maneuver.isValid() ? last_maneuver.sibling(last_maneuver.row(), ManeuverItem::Speed).data().toInt() : turn_model->getStartingSpeed();
    // QString prev_tolerance = last_turn.isValid() ? last_turn.sibling(last_turn.row(), TurnItem::Turn_Tolerance_Tag).data().toString() : "L";
    // bool has_unrestricted_maneuvers = pilot_idx.sibling(pilot_idx.row(), CrewItem::Has_Unrestricted_Maneuvers).data().toBool();

    // // If not the first turn, need the pilot's actions from last turn
    // QVariant pilot_action = last_turn.isValid() ? turn_model->lastTurnPilotIndex(TurnCrewItem::Turn_Action_Col).data(Qt::UserRole) : QVariant();
    // QVariant action_decorator = last_turn.isValid() ? turn_model->lastTurnPilotIndex(TurnCrewItem::Turn_Action_Decorator_Col).data() : QVariant();
    // QPersistentModelIndex crew_idx = turn_model->lastTurnPilotIndex(TurnCrewItem::Turn_Crew_Col).data(Qt::UserRole).toPersistentModelIndex();
    // int fire_template = crew_idx.isValid() && crew_idx.model()->rowCount(crew_idx) > 0 ? crew_idx.model()->index(0, GunItem::Fire_Template, crew_idx).data().toInt() : 0;

    // for (int i=0; i<maneuver_proxy_model->rowCount(plane_idx); ++i) {
    //     QModelIndex can_use_maneuver_idx = maneuver_proxy_model->index(i, ManeuverItem::Can_Be_Used, plane_idx);
    //     QModelIndex maneuver_name_idx = maneuver_proxy_model->index(i, ManeuverItem::Maneuver_Name, plane_idx);
    //     int maneuver_speed = can_use_maneuver_idx.sibling(i, ManeuverItem::Speed).data().toInt();
    //     QString maneuver_direction = can_use_maneuver_idx.sibling(i, ManeuverItem::Direction).data().toString();
    //     QString climb_val = can_use_maneuver_idx.sibling(i, ManeuverItem::Climb_Value).data().toString();
    //     QString level_val = can_use_maneuver_idx.sibling(i, ManeuverItem::Level_Value).data().toString();
    //     QString dive_val = can_use_maneuver_idx.sibling(i, ManeuverItem::Dive_Value).data().toString();
    //     bool must_climb = level_val == "-" && dive_val == "-";
    //     bool must_dive = (climb_val == "-" && level_val == "-") || prev_tolerance == "X";

    //     // If the maneuver is the spin maneuver, just always allow it
    //     if (maneuver_name_idx.data().toString() == "0S1") {
    //         maneuver_proxy_model->setData(can_use_maneuver_idx, true);
    //     }
    //     else if (maneuver_name_idx.sibling(maneuver_name_idx.row(), ManeuverItem::Is_Weight_Restricted).data().toBool() &&
    //              plane_idx.sibling(plane_idx.row(), PlaneItemOld::Bombs_Carried).data().toInt() > 0) {
    //         maneuver_proxy_model->setData(can_use_maneuver_idx, false);
    //     }
    //     // If this is the first turn, let the user use whatever maneuver is in range of the starting speed and altitude
    //     else if (last_turn == QModelIndex()) {
    //         maneuver_proxy_model->setData(can_use_maneuver_idx, prev_speed == maneuver_speed);
    //         if ((prev_alt == 0 && must_dive) ||
    //             (prev_alt == plane_idx.sibling(plane_idx.row(), PlaneItemOld::Max_Altitude).data().toInt() && must_climb) ||
    //             (can_use_maneuver_idx.sibling(i, ManeuverItem::Is_Restricted).data().toBool() &&
    //              !has_unrestricted_maneuvers)) {
    //             maneuver_proxy_model->setData(can_use_maneuver_idx, false);
    //         }
    //     }
    //     // On other turns, start by filtering out any maneuver that would be out of reach of the plane due to stability
    //     else {
    //         QString last_direction = last_maneuver.sibling(last_maneuver.row(), ManeuverItem::Direction).data().toString();
    //         // Add L, S, and R depending on what directions are allowed. Can be combined
    //         QString available_directions;

    //         // In the account of a chit effect causing a rudder jam, account for that here
    //         PlaneItemOld::RudderStates rudder_state = plane_idx.sibling(plane_idx.row(), PlaneItemOld::Rudder_State).data().value<PlaneItemOld::RudderStates>();
    //         if (rudder_state == PlaneItemOld::RudderStates::Rudder_Normal) {
    //             if (plane_idx.sibling(plane_idx.row(), PlaneItemOld::Stability).data().toString() == "C") {
    //                 available_directions = "LSR";
    //             }
    //             else if (last_direction == "L") {
    //                 available_directions = "LS";
    //             }
    //             else if (last_direction == "S") {
    //                 available_directions = "LSR";
    //             }
    //             else if (last_direction == "R") {
    //                 available_directions = "SR";
    //             }
    //         }
    //         else if (rudder_state == PlaneItemOld::Rudder_Jammed_Left) {
    //             available_directions = "L";
    //         }
    //         else if (rudder_state == PlaneItemOld::Rudder_Jammed_Right) {
    //             available_directions = "R";
    //         }

    //         // Additional possible constraints for restricted maneuvers
    //         if (can_use_maneuver_idx.sibling(i, ManeuverItem::Is_Restricted).data().toBool() &&
    //                 last_maneuver.data().toString() != "2S1" && last_maneuver.data().toString() != "3S2" &&
    //                 last_maneuver.data().toString() != "4S3" && last_maneuver.data().toString() != "5S4" &&
    //                 !has_unrestricted_maneuvers) {
    //             maneuver_proxy_model->setData(can_use_maneuver_idx, false);
    //         }
    //         // If the maneuver isn't allowed to be selected based on turn direction, exclude it here and move on
    //         else if (!available_directions.contains(maneuver_direction)){
    //             maneuver_proxy_model->setData(can_use_maneuver_idx, false);
    //         }
    //         // If the altitude will be at max or min, exclude maneuvers that would send beyond
    //         else if ((prev_alt == 0 && must_dive) ||
    //                  (prev_alt == plane_idx.sibling(plane_idx.row(), PlaneItemOld::Max_Altitude).data().toInt() && must_climb)) {
    //             maneuver_proxy_model->setData(can_use_maneuver_idx, false);
    //         }
    //         // Next check speed
    //         else {
    //             maneuver_proxy_model->setData(can_use_maneuver_idx, prev_speed - 1 <= maneuver_speed && maneuver_speed <= prev_speed + 1);
    //         }
    //         // If the pilot shot, see if it forces a climb/dive
    //         if (pilot_action.toInt() == TurnCrewItem::Shot_Action) {
    //             // Don't enforce the climb/dive if it's a short burst at range 3
    //             if (~(action_decorator.toInt() & TurnCrewItem::Short_Burst & TurnCrewItem::Range_3) && fire_template == 1) {
    //                 if (climb_val == "-" && action_decorator.toInt() & TurnCrewItem::Target_Above) {
    //                     maneuver_proxy_model->setData(can_use_maneuver_idx, false);
    //                 }
    //                 else if (dive_val == "-" && action_decorator.toInt() & TurnCrewItem::Target_Below) {
    //                     maneuver_proxy_model->setData(can_use_maneuver_idx, false);
    //                 }
    //             }
    //         }
    //     }
    //     updateManeuver(maneuver_name_idx.data().toString());
    // }
}
