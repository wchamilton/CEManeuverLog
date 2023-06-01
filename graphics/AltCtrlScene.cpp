#include "AltCtrlScene.h"
#include "ManeuverModifiers.h"
#include "models/TurnModel.h"
#include "models/PlaneItems.h"

AltCtrlScene::AltCtrlScene(QObject* parent) : QGraphicsScene(parent)
{
}

void AltCtrlScene::setPlane(QPersistentModelIndex plane_idx)
{
    this->plane_idx = plane_idx;
    if (plane_name) {
        delete plane_name;
    }
    if (plane_tolerances) {
        delete plane_tolerances;
    }

    plane_name = addText(plane_idx.data().toString());
    plane_tolerances = addText(QString("Dive:\t %1\nClimb:\t %2\nAltitude:\t %3%4\nStability:\t %5")
                               .arg(plane_idx.sibling(plane_idx.row(), PlaneItem::Rated_Dive).data().toString())
                               .arg(plane_idx.sibling(plane_idx.row(), PlaneItem::Rated_Climb).data().toString())
                               .arg(plane_idx.sibling(plane_idx.row(), PlaneItem::Max_Altitude).data().toString())
                               .arg(plane_idx.sibling(plane_idx.row(), PlaneItem::Can_Return_To_Max_Alt).data().toBool() ? "+" : "")
                               .arg(plane_idx.sibling(plane_idx.row(), PlaneItem::Stability).data().toString()));

    plane_name->setY(panel->boundingRect().bottom() + 10);
    plane_tolerances->setY(plane_name->y() + 30);

    QFont font = plane_name->font();
    font.setPixelSize(16);
    font.setBold(true);
    font.setUnderline(true);
    plane_name->setFont(font);
    font.setPixelSize(14);
    plane_tolerances->setFont(font);
}

void AltCtrlScene::setTurnModel(TurnModel *model)
{
    turn_model = model;
    if (panel) {
        delete panel;
    }
    panel = new ManeuverModifiers();
    panel->setTurnModel(turn_model);
    addItem(panel);
    setManeuver(QModelIndex());
}

int AltCtrlScene::getCurrentAlt() const
{
    return panel->getCurrentAltitude();
}

void AltCtrlScene::setManeuver(QPersistentModelIndex maneuver_idx)
{
    panel->setCurrentManeuver(maneuver_idx);
    calculateAvailableAltitudes(maneuver_idx);
}

void AltCtrlScene::calculateAvailableAltitudes(QPersistentModelIndex current_maneuver)
{
    if (current_maneuver == QModelIndex()) {
        return;
    }

    int selected_altitude = 0;

    // Cache the selected maneuver regardless of number of turns that have passed
    QString maneuver_dive_val = current_maneuver.sibling(current_maneuver.row(), ManeuverItem::Dive_Value).data().toString();
    QString maneuver_level_val = current_maneuver.sibling(current_maneuver.row(), ManeuverItem::Level_Value).data().toString();
    QString maneuver_climb_val = current_maneuver.sibling(current_maneuver.row(), ManeuverItem::Climb_Value).data().toString();

    QModelIndex plane_idx = current_maneuver.parent();
    int max_plane_alt = plane_idx.sibling(plane_idx.row(), PlaneItem::Max_Altitude).data().toInt();

    // If this is the first turn, we need to ensure that we can only pick maneuvers within range of the starting altitude
    QModelIndex prev_turn_idx = turn_model->lastIndex(TurnItem::Turn_Altitude_Col);
    int prev_alt = prev_turn_idx.isValid() ? prev_turn_idx.data(Qt::UserRole).toInt() : turn_model->getStartingAlt();
    selected_altitude = prev_alt; // Climbing and diving should be relative to the last altitude

    // Determine if the player can stay level
    bool can_stay_level = maneuver_level_val == "L" || maneuver_level_val == "X";
    if (turn_model->rowCount() > 0) {
        QModelIndex prev_maneuver_direction_idx = turn_model->lastIndex(TurnItem::Turn_Tolerance_Tag).data(Qt::UserRole).toModelIndex();
        can_stay_level = can_stay_level && prev_maneuver_direction_idx.data().toString() != "X";
    }

    // Determine the minimum altitude the player can dive to
    int min = std::max(maneuver_dive_val == "D1" ? prev_alt - 1 : maneuver_dive_val == "-" ? prev_alt : 0, 0);

    // Determine if the player can climb
    int can_climb_to = plane_idx.sibling(plane_idx.row(), PlaneItem::Rated_Climb).data().toInt() + prev_alt;
    int max_alt = max_plane_alt;

    if (!plane_idx.sibling(plane_idx.row(), PlaneItem::Can_Return_To_Max_Alt).data().toBool()) {
        --max_alt;
    }
    if (maneuver_climb_val == "C1") {
        max_alt = std::min(prev_alt + 1, max_alt);
    }
    else if (maneuver_climb_val == "-") {
        max_alt = prev_alt;
    }
    else if (maneuver_climb_val == "C") {
        max_alt = std::min(max_alt, can_climb_to);
    }

    QList<int> available_altitudes;
    for (int i=0; i<=max_plane_alt; ++i) {
        bool alt_valid = (i == prev_alt) ? can_stay_level : (i >= min && i <= max_alt);
        if (alt_valid) {
            available_altitudes << i;
        }
    }

    if (!prev_turn_idx.isValid()) {
        selected_altitude = prev_alt;
    }
    else if (selected_altitude > max_alt) {
        selected_altitude = max_alt;
    }
    else if (selected_altitude < min) {
        selected_altitude = min;
    }
    if (selected_altitude == prev_alt && !can_stay_level) {
        selected_altitude = prev_alt-1;
    }

    // Update the UI with the available and pre-selected altitude(s)
    panel->setAvailableAltitudes(available_altitudes);
    panel->setCurrentAltitude(selected_altitude);
}
