#include "ManeuverScene.h"

//#include <QtSvg/QGraphicsSvgItem>
#include <QPersistentModelIndex>
#include <QDebug>

#include "ManeuverGraphic.h"
#include "ManeuverModifiers.h"
#include "models/PlaneItems.h"
#include "models/TurnModel.h"
#include "models/PlaneModel.h"
#include "CEManeuvers.h"

ManeuverScene::ManeuverScene(PlaneFilterProxy *maneuver_proxy, QObject *parent) :
    QGraphicsScene(parent),
    maneuver_proxy_model(maneuver_proxy)
{
    for (auto maneuver : master_maneuver_list) {
        ManeuverGraphic::ShiftText shift_val = ManeuverGraphic::Shift_None;
        if (maneuver.name == "1L0" || maneuver.name == "9L2" || maneuver.name == "11L2" || maneuver.name == "28R1") {
            shift_val = ManeuverGraphic::Shift_Left;
        }
        else if (maneuver.name == "1S0" || maneuver.name == "1R0"  || maneuver.name == "0S1" ||
                 maneuver.name == "9R2" || maneuver.name == "11R2" || maneuver.name == "28L1") {
            shift_val = ManeuverGraphic::Shift_Right;
        }
        maneuver_map[maneuver.name] = new ManeuverGraphic(shift_val);
        for (int i=0; i<maneuver.tile_movements.size()-1; ++i) {
            maneuver_map[maneuver.name]->addHex(maneuver.tile_movements.at(i));
        }
        maneuver_map[maneuver.name]->addHex(maneuver.tile_movements.last(),
                                  maneuver.name == "0S1" ? HexTile::Spin_Tile : HexTile::Plane_Icon_Tile,
                                  maneuver.final_rotation);
        addItem(maneuver_map[maneuver.name]);
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
    maneuver_map[maneuver_idx.sibling(maneuver_idx.row(), ManeuverItem::Maneuver_Name).data().toString()]->setModelIndex(maneuver_idx);
}

void ManeuverScene::removeManeuver(QPersistentModelIndex maneuver_idx)
{
    maneuver_map[maneuver_idx.sibling(maneuver_idx.row(), ManeuverItem::Maneuver_Name).data().toString()]->setModelIndex(QPersistentModelIndex());
}

void ManeuverScene::setManeuver(QPersistentModelIndex maneuver_idx)
{
    for (auto key : maneuver_map.keys()) {
        maneuver_map[key]->setModelIndex(maneuver_idx.sibling(maneuver_idx.row(),
            ManeuverItem::Maneuver_Name).data().toString() == key ? maneuver_idx : QPersistentModelIndex());
    }
}

void ManeuverScene::setPlane(QPersistentModelIndex plane_idx)
{
    this->plane_idx = plane_idx;
    const QAbstractItemModel* plane_model = plane_idx.model();
    QMap<QString, QPersistentModelIndex> maneuver_indexes;

    if (plane_idx.isValid()) {
        // Iterate over the child items of the plane index
        for (int i=0; i<plane_model->rowCount(plane_idx); ++i) {
            QPersistentModelIndex index = plane_model->index(i, ManeuverItem::Maneuver_Name, plane_idx);
            QString name = index.data().toString();
            maneuver_indexes[name] = index;
        }
    }

    for (auto key : maneuver_map.keys()) {
        maneuver_map[key]->setSelected(false); // Ensure if the player loads a new plane that it deselects whatever was there
        maneuver_map[key]->setModelIndex(maneuver_indexes.keys().contains(key) ? maneuver_indexes[key] : QPersistentModelIndex());
    }
    update();
}

void ManeuverScene::setTurnModel(TurnModel *model)
{
    turn_model = model;
}

void ManeuverScene::updateManeuver(QString id)
{
    if (maneuver_map.keys().contains(id)) {
        maneuver_map[id]->updateManeuverState();
    }
}

void ManeuverScene::clearSelection()
{
    if (selected_maneuver) {
        selected_maneuver->setSelected(false);
        selected_maneuver = nullptr;
    }
}

void ManeuverScene::applyScheduleBG()
{
    // Add the background as a pixmap
//    QGraphicsSvgItem* background_item = new QGraphicsSvgItem(QString("../CEManeuverLog/graphics/Canvas_AircraftSheet.svg"));
//    background_item->setZValue(-1);
//    addItem(background_item);

    // for now use shitty png while debugging SVG
//    background_item = addPixmap(QPixmap("./graphics/background.png"));
    background_item = addPixmap(QPixmap("../CEManeuverLog/graphics/background.png"));
    background_item->setZValue(-1);
}

void ManeuverScene::positionManeuvers()
{
    maneuver_map["1L0"]->moveBy(175, 35);
    maneuver_map["1S0"]->moveBy(275, 35);
    maneuver_map["1R0"]->moveBy(395, 35);
    maneuver_map["0S1"]->moveBy(388, 529);
    maneuver_map["10L1"]->moveBy(130, 148);
    maneuver_map["8L1"]->moveBy(180, 135);
    maneuver_map["6S1"]->moveBy(247, 130);
    maneuver_map["2S1"]->moveBy(292, 130);
    maneuver_map["7S1"]->moveBy(338, 130);
    maneuver_map["8R1"]->moveBy(385, 133);
    maneuver_map["10R1"]->moveBy(435, 148);
    maneuver_map["9L2"]->moveBy(30, 370);
    maneuver_map["17L2"]->moveBy(65, 320);
    maneuver_map["16L2"]->moveBy(100, 285);
    maneuver_map["15L2"]->moveBy(135, 270);
    maneuver_map["14L2"]->moveBy(180, 250);
    maneuver_map["11L2"]->moveBy(35, 220);
    maneuver_map["12S2"]->moveBy(247, 245);
    maneuver_map["3S2"]->moveBy(292, 245);
    maneuver_map["13S2"]->moveBy(338, 245);
    maneuver_map["9R2"]->moveBy(540, 370);
    maneuver_map["17R2"]->moveBy(510, 320);
    maneuver_map["16R2"]->moveBy(475, 285);
    maneuver_map["15R2"]->moveBy(435, 270);
    maneuver_map["14R2"]->moveBy(390, 250);
    maneuver_map["11R2"]->moveBy(540, 220);
    maneuver_map["24L3"]->moveBy(55, 530);
    maneuver_map["23L3"]->moveBy(85, 475);
    maneuver_map["22L3"]->moveBy(115, 440);
    maneuver_map["21L3"]->moveBy(150, 410);
    maneuver_map["20L3"]->moveBy(185, 385);
    maneuver_map["18S3"]->moveBy(247, 377);
    maneuver_map["4S3"]->moveBy(292, 377);
    maneuver_map["19S3"]->moveBy(338, 377);
    maneuver_map["20R3"]->moveBy(387, 385);
    maneuver_map["21R3"]->moveBy(422, 410);
    maneuver_map["22R3"]->moveBy(452, 440);
    maneuver_map["23R3"]->moveBy(482, 475);
    maneuver_map["24R3"]->moveBy(515, 530);
    maneuver_map["25S4"]->moveBy(247, 531);
    maneuver_map["5S4"]->moveBy(292, 531);
    maneuver_map["26S4"]->moveBy(338, 531);

    // Restricted speed 1
    maneuver_map["28L1"]->moveBy(115, 610);
    maneuver_map["28R1"]->moveBy(455, 610);

    // Restricted speed 2
    maneuver_map["30L2"]->moveBy(125, 650);
    maneuver_map["31L2"]->moveBy(185, 670);
    maneuver_map["29S2"]->moveBy(291, 650);
    maneuver_map["31R2"]->moveBy(385, 670);
    maneuver_map["30R2"]->moveBy(445, 650);

    // Restricted speed 3
    maneuver_map["36L3"]->moveBy(140, 770);
    maneuver_map["34S3"]->moveBy(230, 780);
    maneuver_map["32S3"]->moveBy(270, 780);
    maneuver_map["33S3"]->moveBy(310, 780);
    maneuver_map["35S3"]->moveBy(350, 780);
    maneuver_map["36R3"]->moveBy(430, 770);
}

void ManeuverScene::handleFocusChanges(QGraphicsItem *newFocusItem, QGraphicsItem *oldFocusItem, Qt::FocusReason reason)
{
    Q_UNUSED(oldFocusItem)
    Q_UNUSED(reason)

    if (newFocusItem != nullptr && background_item != newFocusItem){
        for (auto maneuver : maneuver_map) {
            if (maneuver == newFocusItem) {
                maneuver->setSelected(true);
                emit maneuverClicked(maneuver->getIdx());
                selected_maneuver = maneuver;
            }
            else {
                maneuver->setSelected(false);
            }
        }
        update();
    }
}

void ManeuverScene::setManeuversAvailable(bool has_unrestricted_maneuvers)
{
    QModelIndex last_turn = turn_model->lastIndex(TurnItem::Turn_Maneuver_Col);
    QPersistentModelIndex last_maneuver = last_turn.isValid() ? last_turn.data(Qt::UserRole).toPersistentModelIndex() : QPersistentModelIndex();
    int prev_alt = last_turn.isValid() ? last_turn.sibling(last_turn.row(), TurnItem::Turn_Altitude_Col).data(Qt::UserRole).toInt() : turn_model->getStartingAlt();
    int prev_speed = last_maneuver.isValid() ? last_maneuver.sibling(last_maneuver.row(), ManeuverItem::Speed).data().toInt() : turn_model->getStartingSpeed();
    QString prev_tolerance = last_turn.isValid() ? last_turn.sibling(last_turn.row(), TurnItem::Turn_Tolerance_Tag).data().toString() : "";

    for (int i=0; i<maneuver_proxy_model->rowCount(plane_idx); ++i) {
        QModelIndex can_use_maneuver_idx = maneuver_proxy_model->index(i, ManeuverItem::Can_Be_Used, plane_idx);
        int maneuver_speed = can_use_maneuver_idx.sibling(i, ManeuverItem::Speed).data().toInt();
        QString maneuver_direction = can_use_maneuver_idx.sibling(i, ManeuverItem::Direction).data().toString();
        QString climb_val = can_use_maneuver_idx.sibling(i, ManeuverItem::Climb_Value).data().toString();
        QString level_val = can_use_maneuver_idx.sibling(i, ManeuverItem::Level_Value).data().toString();
        QString dive_val = can_use_maneuver_idx.sibling(i, ManeuverItem::Dive_Value).data().toString();
        bool must_climb = level_val == "-" && dive_val == "-";
        bool must_dive = (climb_val == "-" && level_val == "-") || prev_tolerance == "X";

        // If this is the first turn, let the user use whatever maneuver is in range of the starting speed and altitude
        if (last_turn == QModelIndex()) {
            if ((prev_alt == 0 && must_dive) ||
                (prev_alt == plane_idx.sibling(plane_idx.row(), PlaneItem::Max_Altitude).data().toInt() && must_climb) ||
                (can_use_maneuver_idx.sibling(i, ManeuverItem::Is_Restricted).data().toBool() &&
                 !has_unrestricted_maneuvers)) {
                maneuver_proxy_model->setData(can_use_maneuver_idx, false);
            }
        }
        // On other turns, start by filtering out any maneuver that would be out of reach of the plane due to stability
        else {
            QString last_direction = last_maneuver.sibling(last_maneuver.row(), ManeuverItem::Direction).data().toString();
            // Add L, S, and R depending on what directions are allowed. Can be combined
            QString available_directions;
            if (plane_idx.sibling(plane_idx.row(), PlaneItem::Stability).data().toString() == "C") {
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
            if (can_use_maneuver_idx.sibling(i, ManeuverItem::Is_Restricted).data().toBool() &&
                    last_maneuver.data().toString() != "2S1" && last_maneuver.data().toString() != "3S2" &&
                    last_maneuver.data().toString() != "4S3" && last_maneuver.data().toString() != "5S4" &&
                    !has_unrestricted_maneuvers) {
                maneuver_proxy_model->setData(can_use_maneuver_idx, false);
            }
            // If the maneuver isn't allowed to be selected based on turn direction, exclude it here and move on
            else if (!available_directions.contains(maneuver_direction)){
                maneuver_proxy_model->setData(can_use_maneuver_idx, false);
            }
            // Next check speed
            else {
                maneuver_proxy_model->setData(can_use_maneuver_idx, prev_speed - 1 <= maneuver_speed && maneuver_speed <= prev_speed + 1);
            }
        }
        updateManeuver(can_use_maneuver_idx.sibling(i, ManeuverItem::Maneuver_Name).data().toString());
    }
}

//void ManeuverScene::calculateAvailableAltitudes(QPersistentModelIndex current_maneuver)
//{
//    if (current_maneuver == QModelIndex()) {
//        QModelIndex prev_turn_idx = turn_model->lastIndex(TurnItem::Turn_Altitude_Col);
//        selected_altitude = prev_turn_idx.isValid() ? prev_turn_idx.data(Qt::UserRole).toInt() : turn_model->getStartingAlt();
//    }
//    // Cache the selected maneuver regardless of number of turns that have passed
//    QString maneuver_dive_val = current_maneuver.sibling(current_maneuver.row(), ManeuverItem::Dive_Value).data().toString();
//    QString maneuver_level_val = current_maneuver.sibling(current_maneuver.row(), ManeuverItem::Level_Value).data().toString();
//    QString maneuver_climb_val = current_maneuver.sibling(current_maneuver.row(), ManeuverItem::Climb_Value).data().toString();

//    QModelIndex plane_idx = current_maneuver.parent();
//    int max_plane_alt = plane_idx.sibling(plane_idx.row(), PlaneItem::Max_Altitude).data().toInt();

//    // If this is the first turn, we need to ensure that we can only pick maneuvers within range of the starting altitude
//    QModelIndex prev_turn_idx = turn_model->lastIndex(TurnItem::Turn_Altitude_Col);
//    int prev_alt = prev_turn_idx.isValid() ? prev_turn_idx.data(Qt::UserRole).toInt() : turn_model->getStartingAlt();

//    QString prev_direction_tag = turn_model->lastIndex(TurnItem::Turn_Tolerance_Tag).data().toString();
//    // Determine if the player can stay level
//    bool can_stay_level = maneuver_level_val == "L" || maneuver_level_val == "X";
//    if (turn_model->rowCount() > 0) {
//        QModelIndex prev_maneuver_direction_idx = turn_model->lastIndex(TurnItem::Turn_Tolerance_Tag).data(Qt::UserRole).toModelIndex();
//        can_stay_level = can_stay_level && prev_maneuver_direction_idx.data().toString() != "X";
//    }

//    // Determine the minimum altitude the player can dive to
//    int min = std::max(maneuver_dive_val == "D1" ? prev_alt - 1 : maneuver_dive_val == "-" ? prev_alt : 0, 0);

//    // Determine if the player can climb
//    int can_climb_to = plane_idx.sibling(plane_idx.row(), PlaneItem::Rated_Climb).data().toInt() + prev_alt;
//    int max_alt = max_plane_alt;

//    if (!plane_idx.sibling(plane_idx.row(), PlaneItem::Can_Return_To_Max_Alt).data().toBool()) {
//        --max_alt;
//    }
//    if (maneuver_climb_val == "C1") {
//        max_alt = std::min(prev_alt + 1, max_alt);
//    }
//    else if (maneuver_climb_val == "-") {
//        max_alt = prev_alt;
//    }
//    else if (maneuver_climb_val == "C") {
//        max_alt = std::min(max_alt, can_climb_to);
//    }

//    QList<int> available_altitudes;
//    for (int i=0; i<=max_plane_alt; ++i) {
//        bool alt_valid = (i == prev_alt) ? can_stay_level : (i >= min && i <= max_alt);
//        if (alt_valid) {
//            available_altitudes << i;
//        }
//    }

//    if (!prev_turn_idx.isValid()) {
//        selected_altitude = prev_alt;
//    }
//    else if (selected_altitude > max_alt) {
//        selected_altitude = max_alt;
//    }
//    else if (selected_altitude < min) {
//        selected_altitude = min;
//    }
//    if (selected_altitude == prev_alt && !can_stay_level) {
//        selected_altitude = prev_alt-1;
//    }

//    // Update the UI with the available and pre-selected altitude(s)
////    panel->setAvailableAltitudes(available_altitudes);
////    panel->setCurrentAltitude(selected_altitude);
//}
