#include "TurnTrackerDialog.h"
#include "ui_TurnTrackerDialog.h"

#include "CEManeuvers.h"
#include "CrewControls.h"
#include "models/GameModel.h"
#include "models/GameModelItems.h"
#include "graphics/ManeuverScene.h"
#include "graphics/AltCtrlScene.h"
#include "graphics/FiringArcScene.h"

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
    turn_proxy->setRecursiveFilteringEnabled(true);
    turn_proxy->setAutoAcceptChildRows(true);

    // Set up the damage trackers
    ui->engine_grp->setTitle("Engine");
    ui->wing_grp->setTitle("Wing");
    ui->fuselage_grp->setTitle("Fuselage");
    ui->tail_grp->setTitle("Tail");

    // Extract the selected plane index
    QModelIndex game_idx = game_model->gameRootIdx();
    plane_idx = game_idx.sibling(game_idx.row(), GameItem::Game_Plane_Selected).data().toPersistentModelIndex();

    // Init and assign the graphics scenes
    maneuver_scene = new ManeuverScene(maneuver_proxy.data(), maneuver_proxy->mapFromSource(plane_idx), ui->maneuver_gv);
    ui->maneuver_gv->setScene(maneuver_scene);

    alt_ctrl_scene = new AltCtrlScene(plane_idx, ui->alt_ctrl_gv);
    ui->alt_ctrl_gv->setScene(alt_ctrl_scene);

    firing_arc_scene = new FiringArcScene(ui->firing_arc_gv);
    ui->firing_arc_gv->setScene(firing_arc_scene);
    firing_arc_scene->setCurrentGun(ui->gun_select_rot_cb->currentData().toPersistentModelIndex());

    // Iterate over the crew members and populate their controls
    QPersistentModelIndex plane_filtered_idx = crew_proxy->mapFromSource(plane_idx);
    for (int crew_row=0; crew_row<crew_proxy->rowCount(plane_filtered_idx); ++crew_row) {
        QPersistentModelIndex crew_idx = crew_proxy->index(crew_row, PlaneCrewItem::Plane_Crew_Name, plane_filtered_idx);
        CrewControls* cc = new CrewControls(crew_idx, crew_proxy, maneuver_proxy, turn_proxy, ui->crew_tab);
        ui->crew_tab->addTab(cc, crew_idx.sibling(crew_idx.row(), PlaneCrewItem::Plane_Crew_Role).data().toString() + " (" + crew_idx.data().toString() + ")");

        // If the current crew member is also the pilot, store that so we can later check for movement capabilities/restrictions
        if (crew_idx.sibling(crew_idx.row(), PlaneCrewItem::Plane_Crew_Role_ID).data().toInt() == PlaneCrewItem::Pilot) {
            pilot_idx = crew_idx;
        }

        connect(maneuver_scene, &ManeuverScene::maneuverClicked, cc, &CrewControls::applyManeuverRestrictions);

        // Add each of the guns to the firing arc combobox
        for (int gun_row=0; gun_row<crew_proxy->rowCount(crew_idx); ++gun_row) {
            QPersistentModelIndex gun_idx = crew_proxy->index(gun_row, PlaneArmamentsItem::Plane_Armaments_Name, crew_idx);
            ui->gun_select_rot_cb->addItem(QString("%1 (%2)").arg(gun_idx.data().toString(), crew_idx.data().toString()), gun_idx);
        }
    }

    connect(ui->gun_select_rot_cb, &QComboBox::currentIndexChanged, this, [&](){
        QPersistentModelIndex gun_idx = ui->gun_select_rot_cb->currentData().toPersistentModelIndex();
        firing_arc_scene->setCurrentGun(gun_idx);
        ui->gun_pos_sb->setValue(gun_idx.sibling(gun_idx.row(), PlaneArmamentsItem::Plane_Armaments_Gun_Rotation_Current_Pos).data().toInt());
    });
    connect(ui->gun_rot_left_btn, &QPushButton::clicked, this, [&]{ rotateGun(-1); });
    connect(ui->gun_rot_right_btn, &QPushButton::clicked, this, [&]{ rotateGun(1); });
    connect(maneuver_scene, &ManeuverScene::maneuverClicked, this, &TurnTrackerDialog::handleManeuverSelection);
}

TurnTrackerDialog::~TurnTrackerDialog()
{
    delete ui;
}

void TurnTrackerDialog::handleTurnEnd()
{
    for (int i=0; i<ui->crew_tab->count(); ++i) {
        // Trigger widget refreshes for each of the crew
        static_cast<CrewControls*>(ui->crew_tab->widget(i))->saveCrewData();
    }
}

void TurnTrackerDialog::handleManeuverSelection(QModelIndex maneuver_idx)
{
    int current_alt = plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Current_Alt).data().toInt();
    int climb_range = plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Rated_Climb).data().toInt();
    int dive_range = plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Rated_Climb).data().toInt();
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
        dive_range = 0;
    }

    int lowest = std::max(0, current_alt-dive_range);
    int highest = current_alt+climb_range;
    QList<int> alts(highest - lowest + 1);
    std::iota(alts.begin(), alts.end(), lowest);
    alt_ctrl_scene->updateAltitudes(alts, current_alt);
    alt_ctrl_scene->setManeuver(maneuver_idx);
}

void TurnTrackerDialog::updateAvailableAltitudes()
{

}

void TurnTrackerDialog::refreshCrew()
{

}

void TurnTrackerDialog::rotateGun(const int delta)
{
    // Create local copies of stored data for manipulation before re-persisting a (possibly) modified position
    QModelIndex gun_idx = ui->gun_select_rot_cb->currentData().toModelIndex();
    QList<int> pos_range = gun_idx.sibling(gun_idx.row(), PlaneArmamentsItem::Plane_Armaments_Gun_Rotation_Range).data().value<QList<int>>();
    int current_pos = gun_idx.sibling(gun_idx.row(), PlaneArmamentsItem::Plane_Armaments_Gun_Rotation_Current_Pos).data().toInt();
    int pos_last_turn = gun_idx.sibling(gun_idx.row(), PlaneArmamentsItem::Plane_Armaments_Gun_Rotation_Last_Turn).data().toInt();

    // Temporarily add the delta to the current position
    current_pos += delta;

    // Handle wraparounds for setting the new position
    if (current_pos < pos_range.first()) {
        current_pos += pos_range.last();
    }
    else if (current_pos > pos_range.last()) {
        current_pos -= pos_range.last();
    }

    // Only allowed to rotate one position per turn
    if ((std::abs(pos_last_turn - current_pos) > 1) && // Standard rotation
        (std::abs(pos_last_turn + pos_range.last() - current_pos) > 1)) { // Going from last -> first in wraparound
        return;
    }
    // Need to ensure the rotation sticks to the range (with wraparound)
    if (!pos_range.contains(current_pos)) {
        return;
    }

    ui->gun_pos_sb->setValue(current_pos);
    crew_proxy->setData(gun_idx.sibling(gun_idx.row(), PlaneArmamentsItem::Plane_Armaments_Gun_Rotation_Current_Pos), current_pos);
    firing_arc_scene->setGunRotation(current_pos);
}
