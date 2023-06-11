#include "CrewControls.h"
#include "ui_CrewControls.h"
#include "models/PlaneModel.h"
#include "models/TurnModel.h"

#include <QDebug>

CrewControls::CrewControls(PlaneFilterProxy *model, QPersistentModelIndex crew_idx, TurnModel *turn_model, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CrewControls),
    model(model),
    crew_idx(crew_idx),
    turn_model(turn_model)
{
    ui->setupUi(this);

//    // Initialize the comboboxes with the model
    for (int i=0; i<model->rowCount(crew_idx); ++i) {
        QPersistentModelIndex child_idx = model->index(i, 0, crew_idx);
        ui->gun_selection_shoot->addItem(child_idx.data().toString(), child_idx);
        ui->gun_selection_reload->addItem(child_idx.data().toString(), child_idx);
        ui->gun_selection_unjam->addItem(child_idx.data().toString(), child_idx);
    }

    connect(ui->gun_selection_shoot, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CrewControls::refreshGunWidgets);

    // Necessary to set the combobox to the correct index
    ui->gun_selection_shoot->setCurrentIndex(0);
    ui->gun_selection_reload->setCurrentIndex(0);
    ui->gun_selection_unjam->setCurrentIndex(0);

    refreshGunWidgets();

    // Set action as a property so we know what data to fetch, not setting one for shoot as that'll be handled specifically
    ui->no_action_radio->setProperty("action_taken", TurnCrewItem::No_Action);
    ui->shoot_radio->setProperty("action_taken", TurnCrewItem::Shot_Action);
    ui->reload_radio->setProperty("action_taken", TurnCrewItem::Reload_Action);
    ui->unjam_radio->setProperty("action_taken", TurnCrewItem::Unjam_Action);
    ui->observe_radio->setProperty("action_taken", TurnCrewItem::Observe_Action);
    ui->drop_bomb_radio->setProperty("action_taken", TurnCrewItem::Drop_Bomb_Action);

    // Disable gun controls if no guns equipped for the crew member
    if (model->rowCount(crew_idx) == 0) {
        ui->gun_selection_shoot->setDisabled(true);
        ui->gun_selection_reload->setDisabled(true);
        ui->gun_selection_unjam->setDisabled(true);

        ui->shoot_radio->setDisabled(true);
        ui->reload_radio->setDisabled(true);
        ui->unjam_radio->setDisabled(true);
    }

    updateBombState();

    connect(ui->score_kill_btn, &QPushButton::pressed, this, [=]() { ui->kills_spin->setValue(ui->kills_spin->value()+1); });
    connect(ui->score_red_btn, &QPushButton::pressed, this, [=]() { ui->reds_spin->setValue(ui->reds_spin->value()+1); });
    connect(ui->add_wound_btn, &QPushButton::pressed, this, [=]() { ui->wounds->setValue(ui->wounds->value()+1); });
    connect(ui->wounds, &QSlider::valueChanged, this, [=](int value){
        QString colour;
        switch (value) {
            case WoundValues::None: colour = "blue"; break;
            case WoundValues::Light: colour = "yellow"; break;
            case WoundValues::Severe: colour = "red"; break;
            case WoundValues::Dead: colour = "gray"; break;
        }
        setSliderStylesheet(colour);
        applyCVCalcs();
    });

    connect(ui->long_burst_btn, &QPushButton::released, this, [=]() { ui->burst_len->setValue(2); });
    connect(ui->med_burst_btn, &QPushButton::released, this, [=]() { ui->burst_len->setValue(1); });
    connect(ui->short_burst_btn, &QPushButton::released, this, [=]() { ui->burst_len->setValue(0); });
    connect(ui->burst_len, &QSlider::valueChanged, this, [=](int value) {
        if (value >= ui->ammo_box->value()) {
            ui->burst_len->setValue(ui->ammo_box->value()-1);
            return;
        }
        switch (value) {
            case 0: ui->short_burst_btn->setChecked(true); break;
            case 1: ui->med_burst_btn->setChecked(true); break;
            case 2: ui->long_burst_btn->setChecked(true); break;
        }
        applyCVCalcs();
    });
    connect(ui->target_above, &QPushButton::released, this, [=]() { ui->target_alt->setValue(2); });
    connect(ui->target_level, &QPushButton::released, this, [=]() { ui->target_alt->setValue(1); });
    connect(ui->target_below, &QPushButton::released, this, [=]() { ui->target_alt->setValue(0); });
    connect(ui->target_alt, &QSlider::valueChanged, this, [=](int value) {
        // In the future, maybe subclass QSlider to implement a custom function that "disables" certain ticks
        // For now cycle down until shooting below, and then cycle back to the top (above)
        switch (value) {
            case 0: ui->target_below->isEnabled() ? ui->target_below->setChecked(true) : ui->target_alt->setValue(2); break;
            case 1: ui->target_level->isEnabled() ? ui->target_level->setChecked(true) : ui->target_alt->setValue(0); break;
            case 2: ui->target_above->isEnabled() ? ui->target_above->setChecked(true) : ui->target_alt->setValue(1); break;
        }
        applyCVCalcs();
    });
    connect(ui->hexRangeGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked), this, &CrewControls::applyCVCalcs);
    connect(ui->tailed_target, &QCheckBox::clicked, this, &CrewControls::applyCVCalcs);
    connect(ui->shot_at_target, &QCheckBox::clicked, this, &CrewControls::applyCVCalcs);
    connect(ui->deflection, &QCheckBox::clicked, this, &CrewControls::applyCVCalcs);
    connect(ui->target_spinning, &QCheckBox::clicked, this, &CrewControls::applyCVCalcs);
    connect(ui->target_stalled, &QCheckBox::clicked, this, &CrewControls::applyCVCalcs);
}

CrewControls::~CrewControls()
{
    delete ui;
}

std::tuple<QPersistentModelIndex, int, QVariant> CrewControls::getChosenCrewAction()
{
    int action = ui->actionGroup->checkedButton()->property("action_taken").toInt();
    QVariant action_decorator;
    if (action == TurnCrewItem::Shot_Action) {
        int bitfield = 0;
        // Burst len
        bitfield |= ui->long_burst_btn->isChecked() ? TurnCrewItem::Long_Burst : ui->med_burst_btn->isChecked() ? TurnCrewItem::Medium_Burst : TurnCrewItem::Short_Burst;
        // Angle
        bitfield |= ui->target_level->isChecked() ? TurnCrewItem::Target_Level : ui->target_above->isChecked() ? TurnCrewItem::Target_Above : TurnCrewItem::Target_Below;
        // Range
        bitfield |= ui->range_0_btn->isChecked() ? TurnCrewItem::Range_0 : ui->range_1_btn->isChecked() ? TurnCrewItem::Range_1 : ui->range_2_btn->isChecked() ? TurnCrewItem::Range_2 : TurnCrewItem::Range_3;
        action_decorator = bitfield;
    }
    if (action == TurnCrewItem::Custom_Action) {
        action_decorator = ui->custom_input->text();
    }
    return {crew_idx, action, action_decorator};
}

void CrewControls::handleTurnEnd()
{
    if (ui->shoot_radio->isChecked()) {
        QPersistentModelIndex idx = ui->gun_selection_shoot->currentData().toPersistentModelIndex();
        model->setData(idx.sibling(idx.row(), GunItem::Shots_Fired), ui->burst_len->value()+1);
    }
    else if (ui->reload_radio->isChecked()) {
        QPersistentModelIndex idx = ui->gun_selection_reload->currentData().toPersistentModelIndex();
        model->setData(idx.sibling(idx.row(), GunItem::Ammo_In_Current_Box),
                       idx.sibling(idx.row(), GunItem::Ammo_Box_Capacity).data().toInt());

        QModelIndex ammo_box_count_idx = idx.sibling(idx.row(), GunItem::Ammo_Box_Count);
        model->setData(ammo_box_count_idx, ammo_box_count_idx.data().toInt()-1);
    }
    else if (ui->drop_bomb_radio->isChecked()) {
        QModelIndex plane_bombs_idx = crew_idx.parent().sibling(crew_idx.parent().row(), PlaneItem::Bombs_Carried);
        if (plane_bombs_idx.data().toInt() > 0) {
            model->setData(plane_bombs_idx, plane_bombs_idx.data().toInt() -1);
            // Need to update the rest of the crew that a bomb was dropped
            emit bombDropped();
        }
    }
    refreshGunWidgets();

    QModelIndexList guns;
    for (int i=0; i<ui->gun_selection_shoot->count(); ++i) {
        guns << ui->gun_selection_shoot->itemData(i).toModelIndex();
    }

    ui->gun_selection_shoot->blockSignals(true);
    ui->gun_selection_unjam->blockSignals(true);
    ui->gun_selection_reload->blockSignals(true);

    ui->gun_selection_shoot->clear();
    ui->gun_selection_unjam->clear();
    ui->gun_selection_reload->clear();

    for (auto gun : guns) {
        ui->gun_selection_shoot->addItem(gun.data().toString(), gun);
        ui->gun_selection_unjam->addItem(gun.data().toString(), gun);
        ui->gun_selection_reload->addItem(gun.data().toString(), gun);
    }

    ui->gun_selection_shoot->setCurrentIndex(0);
    ui->gun_selection_reload->setCurrentIndex(0);
    ui->gun_selection_unjam->setCurrentIndex(0);

    ui->gun_selection_shoot->blockSignals(false);
    ui->gun_selection_unjam->blockSignals(false);
    ui->gun_selection_reload->blockSignals(false);

    ui->no_action_radio->setChecked(true);

    selected_maneuver = QPersistentModelIndex();
}

void CrewControls::updateBombState()
{
    // Bombs can only be dropped if the plane has any (left) on board
    ui->drop_bomb_radio->setEnabled(crew_idx.parent().sibling(crew_idx.parent().row(), PlaneItem::Bombs_Carried).data().toInt() > 0 &&
                                    crew_idx.sibling(crew_idx.row(), CrewItem::Can_Drop_Bombs).data().toBool());
    ui->remaining_bombs_lbl->setText(tr("%1 available").arg(model->index(crew_idx.parent().row(), PlaneItem::Bombs_Carried).data().toInt()));
}

void CrewControls::applyManeuverRestrictions(QPersistentModelIndex maneuver_idx)
{
    if (crew_idx.sibling(crew_idx.row(), CrewItem::Crew_Role).data().toString() == "Pilot" ||
            crew_idx.sibling(crew_idx.row(), CrewItem::Crew_Role).data().toString() == "Co-Pilot") {
        QString climb_value = maneuver_idx.sibling(maneuver_idx.row(), ManeuverItem::Climb_Value).data().toString();
        QString level_value = maneuver_idx.sibling(maneuver_idx.row(), ManeuverItem::Level_Value).data().toString();
        QString dive_value  = maneuver_idx.sibling(maneuver_idx.row(), ManeuverItem::Dive_Value).data().toString();

        QModelIndex last_turn_idx = turn_model->lastIndex(TurnItem::Turn_Altitude_Col);

        ui->target_above->setDisabled(climb_value == "-");
        ui->target_level->setDisabled(level_value == "-");
        ui->target_below->setDisabled(dive_value  == "-" || (last_turn_idx.isValid() ? last_turn_idx.data(Qt::UserRole).toInt() : turn_model->getStartingAlt()) <= 1);

        ui->target_above->setChecked(ui->target_above->isEnabled());
        ui->target_level->setChecked(ui->target_level->isEnabled());
        ui->target_below->setChecked(ui->target_below->isEnabled());

        // If the maneuver doesn't allow reloading, then disable the radio selection
        QPersistentModelIndex reload_idx = ui->gun_selection_reload->currentData().toPersistentModelIndex();
        ui->reload_radio->setEnabled(reload_idx.sibling(reload_idx.row(), GunItem::Ammo_Box_Count).data().toInt() > 1 &&
                reload_idx.sibling(reload_idx.row(), GunItem::Ammo_In_Current_Box).data().toInt() == 0 &&
                maneuver_idx.sibling(maneuver_idx.row(), ManeuverItem::Can_Reload).data().toBool());
    }
    else {
        // If the maneuver doesn't allow reloading, then disable the radio selection
        QPersistentModelIndex reload_idx = ui->gun_selection_reload->currentData().toPersistentModelIndex();
        ui->reload_radio->setEnabled(reload_idx.sibling(reload_idx.row(), GunItem::Ammo_Box_Count).data().toInt() > 1 &&
                reload_idx.sibling(reload_idx.row(), GunItem::Ammo_In_Current_Box).data().toInt() == 0 &&
                maneuver_idx.sibling(maneuver_idx.row(), ManeuverItem::Observer_Can_Reload).data().toBool());
    }

    selected_maneuver = maneuver_idx;
}

void CrewControls::setSliderStylesheet(QString colour)
{
    QString stylesheet;
    stylesheet = QString(
        "QSlider::groove:horizontal {"
        "border: 1px solid #bbb;"
        "background: white;"
        "height: 4px;"
        "border-radius: 4px;"
        "}"

        "QSlider::sub-page:horizontal {"
        "background-color: %1;"
        "border: 1px solid #777;"
        "height: 4px;"
        "border-radius: 4px;"
        "}"

        "QSlider::add-page:horizontal {"
        "background: #fff;"
        "border: 1px solid #777;"
        "height: 4px;"
        "border-radius: 4px;"
        "}"

        "QSlider::handle:horizontal {"
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #eee, stop:1 #ccc);"
        "border: 1px solid #777;"
        "width: 8px;"
        "margin-top: -6px;"
        "margin-bottom: -6px;"
        "border-radius: 4px;"
        "}"

        "QSlider::sub-page:horizontal:disabled {"
        "background: #bbb;"
        "border-color: #999;"
        "}"

        "QSlider::add-page:horizontal:disabled {"
        "background: #eee;"
        "border-color: #999;"
        "}"

        "QSlider::handle:horizontal:disabled {"
        "background: #eee;"
        "border: 1px solid #aaa;"
        "border-radius: 4px;"
        "}").arg(colour);
    ui->wounds->setStyleSheet(stylesheet);
}

void CrewControls::refreshGunWidgets()
{
    QPersistentModelIndex shoot_idx = ui->gun_selection_shoot->currentData().toPersistentModelIndex();

    ui->fire_base_3->setText(shoot_idx.sibling(shoot_idx.row(), GunItem::Fire_Base_3).data().toString());
    ui->fire_base_2->setText(shoot_idx.sibling(shoot_idx.row(), GunItem::Fire_Base_2).data().toString());
    ui->fire_base_1->setText(shoot_idx.sibling(shoot_idx.row(), GunItem::Fire_Base_1).data().toString());
    ui->fire_base_0->setText(shoot_idx.sibling(shoot_idx.row(), GunItem::Fire_Base_0).data().toString());

    ui->ammo_box->setValue(shoot_idx.sibling(shoot_idx.row(), GunItem::Ammo_In_Current_Box).data().toInt());
    ui->ammo_box->setSuffix("/" + shoot_idx.sibling(shoot_idx.row(), GunItem::Ammo_Box_Capacity).data().toString());
    ui->ammo_total->setValue(shoot_idx.sibling(shoot_idx.row(), GunItem::Total_Ammo_Remaining).data().toInt());
    ui->ammo_total->setSuffix("/" + shoot_idx.sibling(shoot_idx.row(), GunItem::Total_Ammo).data().toString());

    // Restrict burst length based on remaining ammo
    if (ui->ammo_box->value() < 3) {
        ui->long_burst_btn->setEnabled(false);
        ui->med_burst_btn->setChecked(true);
        if (ui->ammo_box->value() < 2) {
            ui->med_burst_btn->setEnabled(false);
            ui->short_burst_btn->setChecked(true);
        }
        ui->burst_len->setValue(ui->ammo_box->value()-1);
    }
    else {
        ui->long_burst_btn->setEnabled(true);
        ui->long_burst_btn->setChecked(true);
        ui->med_burst_btn->setEnabled(true);
        ui->burst_len->setValue(2);
    }

    // Only allow shooting if there's ammo in the current box
    ui->shoot_radio->setEnabled(shoot_idx.sibling(shoot_idx.row(), GunItem::Ammo_In_Current_Box).data().toInt() > 0);

    // Only allow reloading if there's more than the current ammo box remaining and if all the current ammo has been expended
    QPersistentModelIndex reload_idx = ui->gun_selection_shoot->currentData().toPersistentModelIndex();
    ui->reload_radio->setEnabled(reload_idx.sibling(reload_idx.row(), GunItem::Ammo_Box_Count).data().toInt() > 1 && !ui->shoot_radio->isEnabled());
}

void CrewControls::applyCVCalcs()
{
    ui->combat_value->setValue(calculateCV());
}

int CrewControls::calculateCV()
{
    if (!ui->shoot_radio->isChecked()) {
        return 0;
    }

    // Cache range
    int hex_range = ui->range_0_btn->isChecked() ? 0 : ui->range_1_btn->isChecked() ? 1 : ui->range_2_btn->isChecked() ? 2 : 3;

    // First part of the combat value is the fire base
    QPersistentModelIndex gun_idx = ui->gun_selection_shoot->currentData().toPersistentModelIndex();
    int combat_value = gun_idx.sibling(gun_idx.row(), GunItem::Fire_Base_0 + hex_range).data().toInt();

    // Check if the crew has Ignore Deflection
    bool ignores_deflection = crew_idx.sibling(crew_idx.row(), CrewItem::Has_Ignore_Deflection).data().toBool();

    // DOUBLE CHECK THAT SPINNING TARGETS CANNOT ALSO BE STALLED
    switch (hex_range) {
    case 0: {
        if ((!ignores_deflection && ui->deflection->isChecked()) || ui->target_spinning->isChecked() || ui->target_stalled->isChecked()) {
            return 0;
        }

        combat_value += ui->short_burst_btn->isChecked() ? 0 : ui->med_burst_btn->isChecked() ? 2 : 4;
        combat_value += ui->target_below->isChecked() ? 1 : ui->target_above->isChecked() ? -1 : 0;
        if (ui->tailed_target->isChecked() &&
                crew_idx.sibling(crew_idx.row(), CrewItem::Crew_Role).data().toString() != "Observer" &&
                crew_idx.sibling(crew_idx.row(), CrewItem::Crew_Role).data().toString() != "Gunner") {
            combat_value += 2;
        }
        combat_value += ui->shot_at_target->isChecked() ? 1 : 0;
        combat_value += ui->wounds->value() == 0 ? 0 : ui->wounds->value() == 1 ? -1 : -3;
        break;
    }
    case 1: {
        combat_value += ui->short_burst_btn->isChecked() ? 0 : ui->med_burst_btn->isChecked() ? 2 : 3;
        combat_value += ui->target_spinning->isChecked() ? -6 : !ignores_deflection && ui->deflection->isChecked() ? -1 : 0;
        combat_value += ui->target_below->isChecked() ? 1 : ui->target_above->isChecked() ? -1 : 0;
        combat_value += ui->target_stalled->isChecked() ? 3 : 0;
        if (selected_maneuver.isValid()) {
            if (selected_maneuver.sibling(selected_maneuver.row(), ManeuverItem::Is_Restricted).data().toBool()) {
                combat_value += -1;
            }
            if (selected_maneuver.sibling(selected_maneuver.row(), ManeuverItem::Speed).data().toInt() > 2) {
                combat_value += -1;
            }
        }
        if (ui->tailed_target->isChecked() &&
                crew_idx.sibling(crew_idx.row(), CrewItem::Crew_Role).data().toString() != "Observer" &&
                crew_idx.sibling(crew_idx.row(), CrewItem::Crew_Role).data().toString() != "Gunner") {
            combat_value += 2;
        }
        combat_value += ui->shot_at_target->isChecked() ? 1 : 0;
        combat_value += ui->wounds->value() == 0 ? 0 : ui->wounds->value() == 1 ? -1 : -3;
        break;
    }
    case 2: {
        combat_value += ui->short_burst_btn->isChecked() ? 0 : ui->med_burst_btn->isChecked() ? 1 : 2;
        combat_value += ui->target_spinning->isChecked() ? -7 : !ignores_deflection && ui->deflection->isChecked() ? -2 : 0;
        combat_value += ui->target_below->isChecked() ? 1 : ui->target_above->isChecked() ? -1 : 0;
        combat_value += ui->target_stalled->isChecked() ? 2 : 0;
        if (selected_maneuver.isValid()) {
            if (selected_maneuver.sibling(selected_maneuver.row(), ManeuverItem::Is_Restricted).data().toBool()) {
                combat_value += -2;
            }
            if (selected_maneuver.sibling(selected_maneuver.row(), ManeuverItem::Speed).data().toInt() > 2) {
                combat_value += -2;
            }
        }
        if (ui->tailed_target->isChecked() &&
                crew_idx.sibling(crew_idx.row(), CrewItem::Crew_Role).data().toString() != "Observer" &&
                crew_idx.sibling(crew_idx.row(), CrewItem::Crew_Role).data().toString() != "Gunner") {
            combat_value += 1;
        }
        combat_value += ui->shot_at_target->isChecked() ? 1 : 0;
        combat_value += ui->wounds->value() == 0 ? 0 : ui->wounds->value() == 1 ? -1 : -3;
        break;
    }
    case 3: {
        if (ui->target_spinning->isChecked() || ui->wounds->value() >= 1) {
            return 0;
        }

        combat_value += ui->short_burst_btn->isChecked() ? 0 : ui->med_burst_btn->isChecked() ? 1 : 2;
        combat_value += !ignores_deflection && ui->deflection->isChecked() ? -3 : 0;
        combat_value += ui->target_below->isChecked() ? 0 : ui->target_above->isChecked() ? -2 : 0;
        combat_value += ui->target_stalled->isChecked() ? 2 : 0;
        if (selected_maneuver.isValid()) {
            if (selected_maneuver.sibling(selected_maneuver.row(), ManeuverItem::Is_Restricted).data().toBool()) {
                combat_value += -3;
            }
            if (selected_maneuver.sibling(selected_maneuver.row(), ManeuverItem::Speed).data().toInt() > 2) {
                combat_value += -3;
            }
        }
        if (ui->tailed_target->isChecked() &&
                crew_idx.sibling(crew_idx.row(), CrewItem::Crew_Role).data().toString() != "Observer" &&
                crew_idx.sibling(crew_idx.row(), CrewItem::Crew_Role).data().toString() != "Gunner") {
            combat_value += 1;
        }
        combat_value += ui->wounds->value() == 0 ? 0 : -1;
        break;
    }
    }

    return combat_value;
}
