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
    QPersistentModelIndex plane_filtered_idx = crew_proxy->mapFromSource(plane_idx);
    for (int crew_row=0; crew_row<crew_proxy->rowCount(plane_filtered_idx); ++crew_row) {
        QPersistentModelIndex crew_idx = crew_proxy->index(crew_row, PlaneCrewItem::Plane_Crew_Name, plane_filtered_idx);
        CrewControls* cc = new CrewControls(crew_idx, crew_proxy, maneuver_proxy, ui->crew_tab);
        ui->crew_tab->addTab(cc, crew_idx.sibling(crew_idx.row(), PlaneCrewItem::Plane_Crew_Role).data().toString() + " (" + crew_idx.data().toString() + ")");

        // If the current crew member is also the pilot, store that so we can later check for movement capabilities/restrictions
        if (crew_idx.sibling(crew_idx.row(), PlaneCrewItem::Plane_Crew_Role_ID).data().toInt() == PlaneCrewItem::Pilot) {
            pilot_idx = crew_idx;
        }

        connect(maneuver_scene, &ManeuverScene::maneuverClicked, cc, &CrewControls::applyManeuverRestrictions);

        // Add each of the guns to the firing arc combobox
        for (int gun_row=0; gun_row<crew_proxy->rowCount(crew_idx); ++gun_row) {
            QPersistentModelIndex gun_idx = crew_proxy->index(gun_row, PlaneArmamentsItem::Plane_Armaments_Name, crew_idx);
            ui->gun_rotation_tab->addTab(new GunRotationControl(crew_proxy, gun_idx, ui->gun_rotation_tab), QString("%1 (%2)").arg(gun_idx.data().toString(), crew_idx.data().toString()));
        }
    }

    ui->status_effects_grpbox->layout()->addWidget(new StatusEffectsInterface(crew_proxy, plane_filtered_idx, ui->status_effects_grpbox));
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
        if (auto tab = dynamic_cast<CrewControls*>(ui->crew_tab->widget(i))) {
            tab->saveCrewData();

        }
    }
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
        dive_range = 0;
    }

    int lowest = std::max(0, current_alt-dive_range);
    int highest = current_alt+climb_range;
    QList<int> alts(highest - lowest + 1);
    std::iota(alts.begin(), alts.end(), lowest);
    alt_ctrl_scene->updateAltitudes(alts, current_alt);
    alt_ctrl_scene->setManeuver(maneuver_idx);
}
