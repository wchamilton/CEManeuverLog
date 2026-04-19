#include "GunRotationControl.h"
#include "ui_GunRotationControl.h"
#include "graphics/FiringArcScene.h"
#include "models/GameModel.h"
#include "models/GameModelItems.h"

GunRotationControl::GunRotationControl(QSharedPointer<FilterProxy> crew_proxy, QPersistentModelIndex gun_idx, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GunRotationControl),
    crew_proxy(crew_proxy),
    gun_idx(gun_idx)
{
    ui->setupUi(this);

    firing_arc_scene = new FiringArcScene(ui->firing_arc_gv);
    ui->firing_arc_gv->setScene(firing_arc_scene);
    firing_arc_scene->setCurrentGun(gun_idx);

    connect(ui->gun_rot_left_btn, &QPushButton::clicked, this, [&]{ rotateGun(-1); });
    connect(ui->gun_rot_right_btn, &QPushButton::clicked, this, [&]{ rotateGun(1); });
}

GunRotationControl::~GunRotationControl()
{
    delete ui;
}

void GunRotationControl::saveGunRotation()
{
    crew_proxy->setData(gun_idx.sibling(gun_idx.row(), PlaneArmamentsItem::Plane_Armaments_Gun_Rotation_Last_Turn), ui->gun_pos_sb->value());
}

void GunRotationControl::rotateGun(const int &delta)
{
    // Create local copies of stored data for manipulation before re-persisting a (possibly) modified position
    QList<int> pos_range = gun_idx.sibling(gun_idx.row(), PlaneArmamentsItem::Plane_Armaments_Gun_Rotation_Range).data().value<QList<int>>();
    int current_pos = gun_idx.sibling(gun_idx.row(), PlaneArmamentsItem::Plane_Armaments_Gun_Rotation_Current_Pos).data().toInt();
    int pos_last_turn = gun_idx.sibling(gun_idx.row(), PlaneArmamentsItem::Plane_Armaments_Gun_Rotation_Last_Turn).data().toInt();

    // Temporarily add the delta to the current position
    current_pos += delta;

    // Handle wraparounds for setting the new position
    if (current_pos < pos_range.first()) {
        current_pos += pos_range.last();
    }
    else if (current_pos > pos_range.last()) {
        current_pos -= pos_range.last();
    }

    // Only allowed to rotate one position per turn
    if ((std::abs(pos_last_turn - current_pos) > 1) && // Standard rotation
        (std::abs(pos_last_turn + pos_range.last() - current_pos) > 1)) { // Going from last -> first in wraparound
        return;
    }
    // Need to ensure the rotation sticks to the range (with wraparound)
    if (!pos_range.contains(current_pos)) {
        return;
    }

    ui->gun_pos_sb->setValue(current_pos);
    crew_proxy->setData(gun_idx.sibling(gun_idx.row(), PlaneArmamentsItem::Plane_Armaments_Gun_Rotation_Current_Pos), current_pos);
    firing_arc_scene->setGunRotation(current_pos);
}
