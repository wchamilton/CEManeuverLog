#include "CrewControls.h"
#include "ui_CrewControls.h"
#include "models/PlaneModel.h"

CrewControls::CrewControls(PlaneFilterProxy *model, QPersistentModelIndex crew_idx, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CrewControls),
    model(model),
    crew_idx(crew_idx)
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

    connect(ui->score_kill_btn, &QPushButton::pressed, this, [&]() { ui->kills_spin->setValue(ui->kills_spin->value()+1); });
    connect(ui->score_red_btn, &QPushButton::pressed, this, [&]() { ui->reds_spin->setValue(ui->reds_spin->value()+1); });
    connect(ui->add_wound_btn, &QPushButton::pressed, this, [&]() { ui->wounds->setValue(ui->wounds->value()+1); });
    connect(ui->wounds, &QSlider::valueChanged, this, [&](int value){
        QString colour;
        switch (value) {
            case WoundValues::None: colour = "blue"; break;
            case WoundValues::Light: colour = "yellow"; break;
            case WoundValues::Severe: colour = "red"; break;
            case WoundValues::Dead: colour = "gray"; break;
        }
        setSliderStylesheet(colour);
    });
    connect(ui->actionGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked), this, &CrewControls::updateSelectedAction);

    connect(ui->long_burst_btn, &QPushButton::released, this, [&]() { ui->burst_len->setValue(2); });
    connect(ui->med_burst_btn, &QPushButton::released, this, [&]() { ui->burst_len->setValue(1); });
    connect(ui->short_burst_btn, &QPushButton::released, this, [&]() { ui->burst_len->setValue(0); });
    connect(ui->burst_len, &QSlider::valueChanged, this, [&](int value) {
        switch (value) {
            case 0: ui->short_burst_btn->setChecked(true); break;
            case 1: ui->med_burst_btn->setChecked(true); break;
            case 2: ui->long_burst_btn->setChecked(true); break;
        }
        emit updateSelectedAction();
    });
    connect(ui->custom_action_line_edit, &QLineEdit::textEdited, this, [&](){
        ui->custom_radio->setChecked(true);
        emit updateSelectedAction();
    });
}

CrewControls::~CrewControls()
{
    delete ui;
}

QString CrewControls::getChosenAction()
{
    switch (ui->actionGroup->checkedButton()->property("action_taken").toInt()) {
        case No_Action: return "No action";
        case Shoot_Action: {
            switch (ui->burst_len->value()) {
                case 0: return "Shot a short burst";
                case 1: return "Shot a medium burst";
                case 2: return "Shot a long burst";
            }
            break;
        }
        case Reload_Action: return "Reloaded gun";
        case Unjam_Action: return "Unjammed gun";
        case Observe_Action: return "Observed tile";
        case Drop_Payload_Action: return "Dropped payload";
        case Custom_Action: return ui->custom_action_line_edit->text();
    }
    return QString();
}

void CrewControls::handleTurnEnd()
{
    if (ui->shoot_radio->isChecked()) {
        model->setData(model->index(ui->gun_selection_shoot->currentIndex(), GunItem::Ammo_In_Current_Box, crew_idx), ui->ammo_box_current->text().toInt() - (ui->burst_len->value()+1));
        refreshGunWidgets(ui->gun_selection_shoot->currentIndex());
    }
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

    ui->ammo_box_current->setText(gun_idx.sibling(gun_idx.row(), GunItem::Ammo_In_Current_Box).data().toString());
    ui->ammo_total->setText(QVariant(gun_idx.sibling(gun_idx.row(), GunItem::Ammo_Box_Capacity).data().toInt() *
                                     (gun_idx.sibling(gun_idx.row(), GunItem::Ammo_Box_Count).data().toInt()-1) +
                                     gun_idx.sibling(gun_idx.row(), GunItem::Ammo_In_Current_Box).data().toInt()).toString());

    // Only allow shooting if there's ammo in the current box
    ui->shoot_radio->setEnabled(gun_idx.sibling(gun_idx.row(), GunItem::Ammo_In_Current_Box).data().toInt() > 0);

    // Only allow reloading if there's more than the current ammo box remaining
    ui->reload_radio->setEnabled(gun_idx.sibling(gun_idx.row(), GunItem::Ammo_Box_Count).data().toInt() > 1);
}
