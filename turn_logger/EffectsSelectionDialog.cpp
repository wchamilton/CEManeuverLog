#include "EffectsSelectionDialog.h"
#include "ui_EffectsSelectionDialog.h"

#include "models/PlaneItems.h"
#include "models/PlaneModel.h"

EffectsSelectionDialog::EffectsSelectionDialog(PlaneModel* plane_model, QPersistentModelIndex plane_idx, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EffectsSelectionDialog),
    plane_model(plane_model),
    plane_idx(plane_idx)
{
    ui->setupUi(this);

    // iterate over the plane children
    for (int i=0; i<plane_model->rowCount(plane_idx); ++i) {
        QModelIndex crew_idx = plane_model->index(i, 0, plane_idx);
        if (crew_idx.data(Qt::UserRole).toInt() == BaseItem::Crew_Item_Type) {
            // iterate over the crew children
            for (int j=0; j<plane_model->rowCount(crew_idx); ++j) {
                QPersistentModelIndex crew_child_idx = plane_model->index(j, 0, crew_idx);
                if (crew_child_idx.data(Qt::UserRole).toInt() == BaseItem::Gun_Item_Type &&
                        !crew_child_idx.sibling(crew_child_idx.row(), GunItem::Gun_Destroyed).data().toBool()) {
                    ui->gun_selection_cmb->addItem(crew_child_idx.data().toString(), crew_child_idx);
                }
                else if (crew_child_idx.data(Qt::UserRole).toInt() == BaseItem::Gun_Link_Item_Type) {
                    for (int k=0; k<plane_model->rowCount(crew_child_idx); ++k) {
                        QPersistentModelIndex linked_gun_idx = plane_model->index(k, 0, crew_child_idx);
                        if (linked_gun_idx.data(Qt::UserRole).toInt() == BaseItem::Gun_Item_Type &&
                                !linked_gun_idx.sibling(linked_gun_idx.row(), GunItem::Gun_Destroyed).data().toBool()) {
                            ui->gun_selection_cmb->addItem(linked_gun_idx.data().toString(), linked_gun_idx.sibling(k, GunItem::Gun_Destroyed));
                        }
                    }
                }
            }
        }
    }
    ui->gunDestroyedBox->setEnabled(ui->gun_selection_cmb->count() > 0);
}

EffectsSelectionDialog::~EffectsSelectionDialog()
{
    delete ui;
}

void EffectsSelectionDialog::accept()
{
    if (ui->rudderJamBox->isChecked()) {
        plane_model->setData(plane_idx.sibling(plane_idx.row(), PlaneItem::Rudder_State), ui->jammed_left_radio->isChecked() ?
                                 PlaneItem::Rudder_Jammed_Left : PlaneItem::Rudder_Jammed_Right);
        plane_model->setData(plane_idx.sibling(plane_idx.row(), PlaneItem::Rudder_Jam_Duration), ui->jam_duration->value());
    }
    if (ui->gunDestroyedBox->isChecked()) {
        plane_model->setData(ui->gun_selection_cmb->currentData().toModelIndex(), true);
    }
    if (ui->fuelLostBox->isChecked()) {
        QModelIndex fuel_idx = plane_idx.sibling(plane_idx.row(), PlaneItem::Fuel);
        plane_model->setData(fuel_idx, fuel_idx.data().toInt() - ui->fuel_lost->value());
    }
    QDialog::accept();
}
