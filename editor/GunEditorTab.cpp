#include "GunEditorTab.h"
#include "ui_GunEditorTab.h"

#include "models/PlaneItems.h"

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

void GunEditorTab::populateGunItem(GunItem *gun_item)
{
    gun_item->setData(GunItem::Gun_Name, ui->gun_name->text());
    gun_item->setData(GunItem::Fire_Template, ui->fire_template_spin->value());
    gun_item->setData(GunItem::Fire_Base_3, ui->fire_base_3->value());
    gun_item->setData(GunItem::Fire_Base_2, ui->fire_base_2->value());
    gun_item->setData(GunItem::Fire_Base_1, ui->fire_base_1->value());
    gun_item->setData(GunItem::Fire_Base_0, ui->fire_base_0->value());
    gun_item->setData(GunItem::Ammo_Box_Capacity, ui->ammo_per_box->value());
    gun_item->setData(GunItem::Ammo_Box_Count, ui->ammo_box_count->value());
    gun_item->setData(GunItem::Gun_Is_Linked, ui->is_linked->isChecked());
}

void GunEditorTab::populateFromJSON(QJsonObject gun)
{
    ui->gun_name->setText(gun["name"].toString());
    ui->fire_template_spin->setValue(gun["fire_template"].toInt());
    ui->fire_base_3->setValue(gun["fire_base_3"].toInt());
    ui->fire_base_2->setValue(gun["fire_base_2"].toInt());
    ui->fire_base_1->setValue(gun["fire_base_1"].toInt());
    ui->fire_base_0->setValue(gun["fire_base_0"].toInt());
    ui->ammo_per_box->setValue(gun["ammo_box_capacity"].toInt());
    ui->ammo_box_count->setValue(gun["ammo_box_count"].toInt());
    ui->is_linked->setChecked(gun["is_linked"].toBool());
}
