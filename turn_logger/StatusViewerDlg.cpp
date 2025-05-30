#include "StatusViewerDlg.h"
#include "ui_StatusViewerDlg.h"

#include "models/GameModel.h"
#include "models/GameModelItems.h"

#define EFFECT_ROLE Qt::UserRole + 1

static const QString infinity_symbol("\u221E");
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
        QVariant::fromValue(PlaneItem::Effect{ PlaneItem::Effect_Fuel_Tank_Hit_3, 0,
            "Fuel tank hit! (3 fuel)",
            "Fuel tank was hit, 3 fuel was lost."}));

    ui->effect_selection_cmb->addItem("Fuel tank hit! (6 fuel)",
        QVariant::fromValue(PlaneItem::Effect{ PlaneItem::Effect_Fuel_Tank_Hit_6, 0,
            "Fuel tank hit! (6 fuel)",
            "Fuel tank was hit, 6 fuel was lost."}));

    // Iterate over the crew's guns and add them to the dropdown as destructable. Only add them if they're still working!
    for (int crew_row = 0; crew_row<crew_proxy->rowCount(plane_idx); ++crew_row) {
        QModelIndex crew_idx = crew_proxy->index(crew_row, 0, plane_idx);
        for (int gun_row = 0; gun_row<crew_proxy->rowCount(crew_idx); ++gun_row) {
            QModelIndex gun_idx = crew_proxy->index(gun_row, 0, crew_idx);
            if (gun_idx.data(Qt::UserRole).toInt() == BaseItem::Plane_Armaments_Item_Type &&
                !gun_idx.sibling(gun_row, PlaneArmamentsItem::Plane_Armaments_Gun_Destroyed).data().toBool()) {
                ui->effect_selection_cmb->addItem(QString("%1 (link %2) was destroyed").arg(gun_idx.data().toString()).arg(gun_row+1), gun_idx);
            }
            else if (gun_idx.data(Qt::UserRole).toInt() == BaseItem::Plane_Armaments_Link_Item_Type) {
                for (int linked_gun_row = 0; linked_gun_row < crew_proxy->rowCount(gun_idx); ++linked_gun_row) {
                    QModelIndex linked_gun_idx = crew_proxy->index(linked_gun_row, 0, gun_idx);
                    if (linked_gun_idx.data(Qt::UserRole).toInt() == BaseItem::Plane_Armaments_Item_Type &&
                        !linked_gun_idx.sibling(linked_gun_row, PlaneArmamentsItem::Plane_Armaments_Gun_Destroyed).data().toBool()) {
                        ui->effect_selection_cmb->addItem(QString("%1 (link %2) was destroyed").arg(linked_gun_idx.data().toString()).arg(linked_gun_row+1), linked_gun_idx);
                    }
                }
            }
        }
    }

    connect(ui->add_effect_btn, &QPushButton::clicked, this, &StatusViewerDlg::addEffect);
    connect(ui->reset_btn, &QPushButton::clicked, this, &StatusViewerDlg::resetEffects);

    // Triggers the first load of the current effects from the model. Further resets will reset all changes in this instance of the viewer
    resetEffects();
}

StatusViewerDlg::~StatusViewerDlg()
{
    delete ui;
}

void StatusViewerDlg::accept()
{
    crew_proxy->setData(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Active_Effects), QVariant::fromValue(effects));
    for (const QModelIndex &idx : std::as_const(guns_to_destroy)) {
        crew_proxy->setData(idx, true); // The index should be pre-set to PlaneArmamentsItem::Plane_Armaments_Gun_Destroyed
    }
    QDialog::accept();
}

void StatusViewerDlg::addEffect()
{
    // Create an empty effect struct, this will be extracted or constructed after determining if it's a gun to pop
    PlaneItem::Effect effect;

    QVariant selected_effect_data = ui->effect_selection_cmb->currentData();
    if (selected_effect_data.canConvert<PlaneItem::Effect>()) {
        effect = selected_effect_data.value<PlaneItem::Effect>();
    }
    // At this point we assume it's a QModelIndex of a gun to destroy. First populate an Effect object that will get stored for viewing.
    else {
        QModelIndex gun_idx = selected_effect_data.toModelIndex();
        effect.id = PlaneItem::Effect_Gun_Destroyed;
        effect.name = QString("%1 was destroyed").arg(gun_idx.data().toString());
        effect.desc = "This gun was destroyed, it cannot be used for the remainder of the match.";
        effect.remaining_turns = 0; // 0 indicates perpetual
    }

    if (effects.contains(effect)) {
        removeStatusEffectFromTable(effect);
    }

    addStatusEffectToTable(effect);
    ui->effects_table->resizeColumnToContents(0);

    // Update the current list of active effects
    effects.removeAll(effect);
    effects << effect;
}

void StatusViewerDlg::resetEffects()
{
    QModelIndex plane_effects_idx = plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Active_Effects);
    effects = plane_effects_idx.data().value<QList<PlaneItem::Effect>>();
    ui->effects_table->setRowCount(0);
    for (const PlaneItem::Effect &effect : std::as_const(effects)) {
        addStatusEffectToTable(effect);
    }
    ui->effects_table->resizeColumnToContents(0);
}

void StatusViewerDlg::addStatusEffectToTable(const PlaneItem::Effect &effect)
{
    QTableWidgetItem* effect_item = new QTableWidgetItem(effect.name);
    effect_item->setToolTip(effect.desc);
    effect_item->setData(EFFECT_ROLE, QVariant::fromValue(effect));
    QTableWidgetItem* effect_item_duration = new QTableWidgetItem();
    int duration = effect.remaining_turns;
    effect_item_duration->setText(duration == 0 ? infinity_symbol : QString::number(duration));

    int row = ui->effects_table->rowCount();
    ui->effects_table->insertRow(row);
    ui->effects_table->setItem(row, 0, effect_item);
    ui->effects_table->setItem(row, 1, effect_item_duration);
}

void StatusViewerDlg::removeStatusEffectFromTable(const PlaneItem::Effect &effect)
{
    for (int i=0; i<ui->effects_table->rowCount(); ++i) {
        if (ui->effects_table->item(i, 0)->data(EFFECT_ROLE).value<PlaneItem::Effect>() == effect) {
            ui->effects_table->removeRow(i);
            return;
        }
    }
}
