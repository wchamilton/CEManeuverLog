#include "CrewEditorTab.h"
#include "ui_CrewEditorTab.h"

#include "models/PlaneItems.h"

CrewEditorTab::CrewEditorTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CrewEditorTab)
{
    ui->setupUi(this);
}

CrewEditorTab::~CrewEditorTab()
{
    delete ui;
}

void CrewEditorTab::populateCrewItem(CrewItem *crew)
{
    crew->setData(CrewItem::Crew_Role, ui->role_cmb->currentText());
    // Add gun controls
    if (ui->gun_grp->isChecked()) {
        GunItem* gun_item = new GunItem(crew);
        gun_item->setData(GunItem::Gun_Name, ui->gun_name->text());
        gun_item->setData(GunItem::Fire_Template, ui->fire_template_spin->value());
        gun_item->setData(GunItem::Fire_Base_3, ui->fire_base_3->value());
        gun_item->setData(GunItem::Fire_Base_2, ui->fire_base_2->value());
        gun_item->setData(GunItem::Fire_Base_1, ui->fire_base_1->value());
        gun_item->setData(GunItem::Fire_Base_0, ui->fire_base_0->value());
        gun_item->setData(GunItem::Ammo_Box_Capacity, ui->ammo_per_box->value());
        gun_item->setData(GunItem::Ammo_Box_Count, ui ->ammo_box_count->value());
        crew->addChild(gun_item);
    }
}
