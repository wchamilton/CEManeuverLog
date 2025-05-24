#include "StatusViewerDlg.h"
#include "ui_StatusViewerDlg.h"

#include "models/GameModel.h"
#include "models/GameModelItems.h"

StatusViewerDlg::StatusViewerDlg(const QSharedPointer<FilterProxy> &crew_proxy, const QPersistentModelIndex &plane_idx, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StatusViewerDlg),
    crew_proxy(crew_proxy),
    plane_idx(plane_idx)
{
    ui->setupUi(this);

    // Initiallize each of the combobox elements and initialize the effect structures at the same time as custom data
    ui->effect_selection_cmb->addItem("No restricted maneuvers",
        QVariant::fromValue(PlaneItem::Effect{ PlaneItem::Effect_No_Restricted_Maneuvers, 0,
            "No restricted maneuvers",
            "Cannot perform restricted maneuvers for the duration of the match."}));

    ui->effect_selection_cmb->addItem("No speed 3 or 4 maneuvers",
        QVariant::fromValue(PlaneItem::Effect{ PlaneItem::Effect_No_Speed_3_or_4, 0,
            "No speed 3 or 4 maneuvers",
            "Cannot perform speed 3 or 4 maneuvers for the duration of the match."}));

    ui->effect_selection_cmb->addItem("Must do slower maneuver",
        QVariant::fromValue(PlaneItem::Effect{ PlaneItem::Effect_Force_Slower_Maneuver, 1,
            "Must do slower maneuver",
            "Must perform a slower maneuver next turn."}));

    ui->effect_selection_cmb->addItem("Rudder jam (Left - 2 turns)",
        QVariant::fromValue(PlaneItem::Effect{ PlaneItem::Effect_Rudder_Jam_Left, 2,
            "Rudder jam (Left - 2 turns)",
            "Rudder is jammed left for 2 turns."}));

    ui->effect_selection_cmb->addItem("Rudder jam (Left - 3 turns)",
        QVariant::fromValue(PlaneItem::Effect{ PlaneItem::Effect_Rudder_Jam_Left, 3,
            "Rudder jam (Left - 3 turns)",
            "Rudder is jammed left for 3 turns."}));

    ui->effect_selection_cmb->addItem("Rudder jam (Right - 2 turns)",
        QVariant::fromValue(PlaneItem::Effect{ PlaneItem::Effect_Rudder_Jam_Right, 2,
            "Rudder jam (Right - 2 turns)",
            "Rudder is jammed left for 2 turns."}));

    ui->effect_selection_cmb->addItem("Rudder jam (Right - 3 turns)",
        QVariant::fromValue(PlaneItem::Effect{ PlaneItem::Effect_Rudder_Jam_Right, 3,
            "Rudder jam (Right - 3 turns)",
            "Rudder is jammed left for 3 turns."}));

    ui->effect_selection_cmb->addItem("Fuel tank hit! (3 fuel)",
        QVariant::fromValue(PlaneItem::Effect{ PlaneItem::Effect_Fuel_Tank_Hit_3, 3,
            "Fuel tank hit! (3 fuel)",
            "Fuel tank was hit, 3 fuel was lost."}));

    ui->effect_selection_cmb->addItem("Fuel tank hit! (6 fuel)",
        QVariant::fromValue(PlaneItem::Effect{ PlaneItem::Effect_Fuel_Tank_Hit_6, 3,
            "Fuel tank hit! (6 fuel)",
            "Fuel tank was hit, 6 fuel was lost."}));

    connect(ui->add_effect_btn, &QPushButton::clicked, this, [&](){
        addStatusEffectToTable(ui->effect_selection_cmb->currentData());
        ui->effects_table->resizeColumnsToContents();

        effects.removeAll(ui->effect_selection_cmb->currentData());
        effects << ui->effect_selection_cmb->currentData();
    });
    connect(ui->reset_btn, &QPushButton::clicked, this, [&](){
        QModelIndex plane_effects_idx = plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Active_Effects);
        effects = plane_effects_idx.data().toList();
        ui->effects_table->setRowCount(0);
        for (const QVariant &effect : std::as_const(effects)) {
            addStatusEffectToTable(effect);
        }
        ui->effects_table->resizeColumnsToContents();
    });

    // Triggers the first load of the current effects from the model. Further resets will reset all changes in this instance of the viewer
    ui->reset_btn->click();
}

StatusViewerDlg::~StatusViewerDlg()
{
    delete ui;
}

void StatusViewerDlg::accept()
{
    crew_proxy->setData(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Active_Effects), effects);
    QDialog::accept();
}

void StatusViewerDlg::addStatusEffectToTable(const QVariant &effect)
{
    QTableWidgetItem* effect_item = new QTableWidgetItem(effect.value<PlaneItem::Effect>().name);
    effect_item->setToolTip(effect.value<PlaneItem::Effect>().desc);
    QTableWidgetItem* effect_item_duration = new QTableWidgetItem(QString::number(effect.value<PlaneItem::Effect>().remaining_turns));
    int row = ui->effects_table->rowCount();
    ui->effects_table->insertRow(row);
    ui->effects_table->setItem(row, 0, effect_item);
    ui->effects_table->setItem(row, 1, effect_item_duration);
}
