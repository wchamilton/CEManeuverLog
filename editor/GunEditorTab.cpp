#include "GunEditorTab.h"
#include "ui_GunEditorTab.h"

#include "models/GameModel.h"
#include "models/GameModelItems.h"

#include <QJsonObject>

GunEditorTab::GunEditorTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GunEditorTab)
{
    ui->setupUi(this);
}

GunEditorTab::~GunEditorTab()
{
    delete ui;
}

void GunEditorTab::populateGunItem(FilterProxy* model, QPersistentModelIndex gun_item)
{
    model->setData(gun_item.sibling(gun_item.row(), PlaneArmamentsItem::Plane_Armaments_Name), ui->gun_name->text());
    model->setData(gun_item.sibling(gun_item.row(), PlaneArmamentsItem::Plane_Armaments_Fire_Template), ui->fire_template_spin->value());
    model->setData(gun_item.sibling(gun_item.row(), PlaneArmamentsItem::Plane_Armaments_Fire_Base_3), ui->fire_base_3->value());
    model->setData(gun_item.sibling(gun_item.row(), PlaneArmamentsItem::Plane_Armaments_Fire_Base_2), ui->fire_base_2->value());
    model->setData(gun_item.sibling(gun_item.row(), PlaneArmamentsItem::Plane_Armaments_Fire_Base_1), ui->fire_base_1->value());
    model->setData(gun_item.sibling(gun_item.row(), PlaneArmamentsItem::Plane_Armaments_Fire_Base_0), ui->fire_base_0->value());
    model->setData(gun_item.sibling(gun_item.row(), PlaneArmamentsItem::Plane_Armaments_Ammo_Box_Capacity), ui->ammo_per_box->value());
    model->setData(gun_item.sibling(gun_item.row(), PlaneArmamentsItem::Plane_Armaments_Ammo_Box_Count), ui->ammo_box_count->value());
    model->setData(gun_item.sibling(gun_item.row(), PlaneArmamentsItem::Plane_Armaments_Gun_Is_Linked), ui->is_linked->isChecked());
}

void GunEditorTab::populateFromModel(QPersistentModelIndex gun_idx)
{
    ui->gun_name->setText(gun_idx.data().toString());
    ui->fire_template_spin->setValue(gun_idx.sibling(gun_idx.row(), PlaneArmamentsItem::Plane_Armaments_Fire_Template).data().toInt());
    ui->fire_base_3->setValue(gun_idx.sibling(gun_idx.row(), PlaneArmamentsItem::Plane_Armaments_Fire_Base_3).data().toInt());
    ui->fire_base_2->setValue(gun_idx.sibling(gun_idx.row(), PlaneArmamentsItem::Plane_Armaments_Fire_Base_2).data().toInt());
    ui->fire_base_1->setValue(gun_idx.sibling(gun_idx.row(), PlaneArmamentsItem::Plane_Armaments_Fire_Base_1).data().toInt());
    ui->fire_base_0->setValue(gun_idx.sibling(gun_idx.row(), PlaneArmamentsItem::Plane_Armaments_Fire_Base_0).data().toInt());
    ui->ammo_per_box->setValue(gun_idx.sibling(gun_idx.row(), PlaneArmamentsItem::Plane_Armaments_Ammo_Box_Capacity).data().toInt());
    ui->ammo_box_count->setValue(gun_idx.sibling(gun_idx.row(), PlaneArmamentsItem::Plane_Armaments_Ammo_Box_Count).data().toInt());
    ui->is_linked->setChecked(gun_idx.sibling(gun_idx.row(), PlaneArmamentsItem::Plane_Armaments_Gun_Is_Linked).data().toBool());
}
