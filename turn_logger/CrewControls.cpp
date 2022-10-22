#include "CrewControls.h"
#include "ui_CrewControls.h"
#include "models/PlaneModel.h"

CrewControls::CrewControls(PlaneFilterProxy *model, QPersistentModelIndex crew_idx, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CrewControls)
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

    // Necessary to set the combobox to the correct leaf index
    ui->gun_selection_shoot->setCurrentIndex(0);
    ui->gun_selection_reload->setCurrentIndex(0);
    ui->gun_selection_unjam->setCurrentIndex(0);

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

    connect(ui->score_red_btn, &QPushButton::pressed, this, [&]() {
        ui->reds_spin->setValue(ui->reds_spin->value()+1);
    });
    connect(ui->add_wound_btn, &QPushButton::pressed, this, [&]() {
        ui->wounds->setValue(ui->wounds->value()+1);
    });
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
}

CrewControls::~CrewControls()
{
    delete ui;
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

void CrewControls::populateGunControls(QPersistentModelIndex gun_idx)
{

}
