#include "GunControls.h"
#include "ui_GunControls.h"

#include "models/PlaneItems.h"

GunControls::GunControls(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GunControls)
{
    ui->setupUi(this);
}

GunControls::GunControls(QPersistentModelIndex idx, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GunControls)
{
    ui->setupUi(this);

    ui->fire_base_3->setText(idx.sibling(idx.row(), GunItem::Fire_Base_3).data().toString());
    ui->fire_base_2->setText(idx.sibling(idx.row(), GunItem::Fire_Base_2).data().toString());
    ui->fire_base_1->setText(idx.sibling(idx.row(), GunItem::Fire_Base_1).data().toString());
    ui->fire_base_0->setText(idx.sibling(idx.row(), GunItem::Fire_Base_0).data().toString());

    ui->ammo_box_current->setText(idx.sibling(idx.row(), GunItem::Ammo_Box_Capacity).data().toString());
    ui->ammo_total->setText(QVariant(idx.sibling(idx.row(), GunItem::Ammo_Box_Capacity).data().toInt() *
                                     idx.sibling(idx.row(), GunItem::Ammo_Box_Count).data().toInt()).toString());

    connect(ui->long_burst_btn, &QPushButton::released, this, [&]() { ui->burst_len->setValue(2); });
    connect(ui->med_burst_btn, &QPushButton::released, this, [&]() { ui->burst_len->setValue(1); });
    connect(ui->short_burst_btn, &QPushButton::released, this, [&]() { ui->burst_len->setValue(0); });
    connect(ui->burst_len, &QSlider::valueChanged, this, [&](int value) {
        switch (value) {
            case 0: ui->short_burst_btn->setChecked(true); break;
            case 1: ui->med_burst_btn->setChecked(true); break;
            case 2: ui->long_burst_btn->setChecked(true); break;
        }
    });
}

GunControls::~GunControls()
{
    delete ui;
}
