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

    plane_name->setY(panel->boundingRect().bottom() + 50);
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
    if (turn_display == nullptr) {
        turn_display = addText("Turn 1");
        QFont font = turn_display->font();
        font.setPixelSize(18);
        font.setBold(true);
        turn_display->setFont(font);
        connect(turn_model, &TurnModel::rowsInserted, this, [=] { turn_display->setPlainText(QString("Turn %1").arg(turn_model->rowCount()+1)); });
    }
    else {
        turn_display->setPlainText("Turn 1");
    }
    panel = new ManeuverModifiers();
    panel->setTurnModel(turn_model);
    addItem(panel);
    panel->setY(40);
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
    bool must_climb = false;
    bool must_dive = false;
    if (turn_model->rowCount() > 0) {
        QModelIndex prev_maneuver_direction_idx = turn_model->lastIndex(TurnItem::Turn_Tolerance_Tag).data(Qt::UserRole).toModelIndex();
        can_stay_level = can_stay_level && prev_maneuver_direction_idx.data().toString() != "X";

        // If not the first turn, need the pilot's actions from last turn
        QVariant pilot_action = turn_model->lastTurnPilotIndex(TurnCrewItem::Turn_Action_Col).data(Qt::UserRole);
        QVariant action_decorator = turn_model->lastTurnPilotIndex(TurnCrewItem::Turn_Action_Decorator_Col).data();

        // If the pilot shot, see if it forces a climb/dive
        if (pilot_action.toInt() == TurnCrewItem::Shot_Action) {
            // Don't enforce the climb/dive if it's a short burst at range 3 and not a fixed weapon
            QPersistentModelIndex crew_idx = turn_model->lastTurnPilotIndex(TurnCrewItem::Turn_Crew_Col).data(Qt::UserRole).toPersistentModelIndex();
            int fire_template = crew_idx.isValid() && crew_idx.model()->rowCount(crew_idx) > 0 ? crew_idx.model()->index(0, GunItem::Fire_Template, crew_idx).data().toInt() : 0;
            if (~(action_decorator.toInt() & TurnCrewItem::Short_Burst & TurnCrewItem::Range_3) && fire_template == 1) {
                if (action_decorator.toInt() & TurnCrewItem::Target_Above) {
                    must_climb = true;
                    can_stay_level = false;
                }
                else if (action_decorator.toInt() & TurnCrewItem::Target_Below) {
                    must_dive = true;
                    can_stay_level = false;
                }
            }
        }
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

    // apply min/max if the pilot shot with a fixed weapon
    if (must_climb) {
        min = prev_alt+1;
    }
    if (must_dive) {
        max_alt = prev_alt-1;
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
