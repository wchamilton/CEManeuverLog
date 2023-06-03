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
    QDialog::accept();
}
