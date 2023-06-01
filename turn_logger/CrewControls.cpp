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

    // Initialize the comboboxes with the model
    ui->gun_selection_shoot->setModel(model);
    ui->gun_selection_reload->setModel(model);
    ui->gun_selection_unjam->setModel(model);

    // Set the leaf node of the model from which the comboboxes should derive their content
    ui->gun_selection_shoot->setRootModelIndex(crew_idx);
    ui->gun_selection_reload->setRootModelIndex(crew_idx);
    ui->gun_selection_unjam->setRootModelIndex(crew_idx);

    connect(ui->gun_selection_shoot, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CrewControls::refreshGunWidgets);

    // Necessary to set the combobox to the correct leaf index
    ui->gun_selection_shoot->setCurrentIndex(0);
    ui->gun_selection_reload->setCurrentIndex(0);
    ui->gun_selection_unjam->setCurrentIndex(0);

    // Set action as a property so we know what data to fetch
    ui->no_action_radio->setProperty("action_taken", No_Action);
    ui->shoot_radio->setProperty("action_taken", Shoot_Action);
    ui->reload_radio->setProperty("action_taken", Reload_Action);
    ui->unjam_radio->setProperty("action_taken", Unjam_Action);
    ui->observe_radio->setProperty("action_taken", Observe_Action);
    ui->drop_bomb_radio->setProperty("action_taken", Drop_Payload_Action);
    ui->custom_radio->setProperty("action_taken", Custom_Action);

    // Disable gun controls if no guns equipped for the crew member
    if (model->rowCount(crew_idx) == 0) {
        ui->gun_selection_shoot->setDisabled(true);
        ui->gun_selection_reload->setDisabled(true);
        ui->gun_selection_unjam->setDisabled(true);

        ui->shoot_radio->setDisabled(true);
        ui->reload_radio->setDisabled(true);
        ui->unjam_radio->setDisabled(true);
    }

    if (!crew_idx.sibling(crew_idx.row(), CrewItem::Can_Drop_Bombs).data().toBool()) {
        ui->drop_bomb_radio->setDisabled(true);
        ui->remaining_bombs_lbl->setText(tr("%1 available").arg(model->index(crew_idx.parent().row(), PlaneItem::Bombs_Carried).data().toInt()));
    }

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
        switch (value) {
            case 0: ui->target_below->setChecked(true); break;
            case 1: ui->target_level->setChecked(true); break;
            case 2: ui->target_above->setChecked(true); break;
        }
        applyCVCalcs();
    });
    connect(ui->custom_action_line_edit, &QLineEdit::textEdited, this, [=](){ ui->custom_radio->setChecked(true); });
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

QPair<QPersistentModelIndex, QString> CrewControls::getChosenCrewAction()
{
    QString action = "";
    switch (ui->actionGroup->checkedButton()->property("action_taken").toInt()) {
        case No_Action: action = "No action"; break;
        case Shoot_Action: {
            switch (ui->burst_len->value()) {
                case 0: action = "Shot a short burst"; break;
                case 1: action = "Shot a medium burst"; break;
                case 2: action = "Shot a long burst"; break;
            }
            break;
        }
        case Reload_Action: action = "Reloaded gun"; break;
        case Unjam_Action: action = "Unjammed gun"; break;
        case Observe_Action: action = "Observed tile"; break;
        case Drop_Payload_Action: action = "Dropped payload"; break;
        case Custom_Action: action = ui->custom_action_line_edit->text(); break;
    }
    return QPair<QPersistentModelIndex,QString>(crew_idx, action);
}

void CrewControls::handleTurnEnd()
{
    if (ui->shoot_radio->isChecked()) {
        model->setData(model->index(ui->gun_selection_shoot->currentIndex(), GunItem::Ammo_In_Current_Box, crew_idx),
                       std::max(ui->ammo_box->value() - (ui->burst_len->value()+1), 0));
    }
    else if (ui->reload_radio->isChecked()) {
        model->setData(model->index(ui->gun_selection_shoot->currentIndex(), GunItem::Ammo_In_Current_Box, crew_idx),
                       model->index(ui->gun_selection_shoot->currentIndex(), GunItem::Ammo_Box_Capacity, crew_idx).data().toInt());

        QModelIndex ammo_box_count_idx = model->index(ui->gun_selection_shoot->currentIndex(), GunItem::Ammo_Box_Count, crew_idx);
        model->setData(ammo_box_count_idx, ammo_box_count_idx.data().toInt()-1);
    }
    refreshGunWidgets(ui->gun_selection_shoot->currentIndex());
    ui->no_action_radio->setChecked(true);
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

void CrewControls::refreshGunWidgets(int row)
{
    QModelIndex gun_idx = model->index(row, GunItem::Gun_Name, crew_idx);

    ui->fire_base_3->setText(gun_idx.sibling(gun_idx.row(), GunItem::Fire_Base_3).data().toString());
    ui->fire_base_2->setText(gun_idx.sibling(gun_idx.row(), GunItem::Fire_Base_2).data().toString());
    ui->fire_base_1->setText(gun_idx.sibling(gun_idx.row(), GunItem::Fire_Base_1).data().toString());
    ui->fire_base_0->setText(gun_idx.sibling(gun_idx.row(), GunItem::Fire_Base_0).data().toString());

    ui->ammo_box->setValue(gun_idx.sibling(gun_idx.row(), GunItem::Ammo_In_Current_Box).data().toInt());
    ui->ammo_box->setSuffix("/" + gun_idx.sibling(gun_idx.row(), GunItem::Ammo_Box_Capacity).data().toString());
    ui->ammo_total->setValue(gun_idx.sibling(gun_idx.row(), GunItem::Total_Ammo_Remaining).data().toInt());
    ui->ammo_total->setSuffix("/" + gun_idx.sibling(gun_idx.row(), GunItem::Total_Ammo).data().toString());

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
    ui->shoot_radio->setEnabled(gun_idx.sibling(gun_idx.row(), GunItem::Ammo_In_Current_Box).data().toInt() > 0);

    // Only allow reloading if there's more than the current ammo box remaining
    ui->reload_radio->setEnabled(gun_idx.sibling(gun_idx.row(), GunItem::Ammo_Box_Count).data().toInt() > 1);
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
    int combat_value = model->index(ui->gun_selection_shoot->currentIndex(), GunItem::Fire_Base_0 + hex_range, crew_idx).data().toInt();

    QPersistentModelIndex maneuver = turn_model->lastIndex(TurnItem::Turn_Maneuver_Col).data().toPersistentModelIndex();

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
        if (maneuver.isValid()) {
            if (maneuver.sibling(maneuver.row(), ManeuverItem::Is_Restricted).data().toBool()) {
                combat_value += -1;
            }
            if (maneuver.sibling(maneuver.row(), ManeuverItem::Speed).data().toInt() > 2) {
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
        if (maneuver.isValid()) {
            if (maneuver.sibling(maneuver.row(), ManeuverItem::Is_Restricted).data().toBool()) {
                combat_value += -2;
            }
            if (maneuver.sibling(maneuver.row(), ManeuverItem::Speed).data().toInt() > 2) {
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
        if (maneuver.isValid()) {
            if (maneuver.sibling(maneuver.row(), ManeuverItem::Is_Restricted).data().toBool()) {
                combat_value += -3;
            }
            if (maneuver.sibling(maneuver.row(), ManeuverItem::Speed).data().toInt() > 2) {
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
