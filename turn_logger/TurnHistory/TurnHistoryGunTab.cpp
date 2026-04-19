#include "TurnHistoryGunTab.h"
#include "ui_TurnHistoryGunTab.h"

#include <QAbstractItemModel>
#include "models/GameModelItems.h"
#include "graphics/FiringArcScene.h"

TurnHistoryGunTab::TurnHistoryGunTab(QPersistentModelIndex gun_idx, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TurnHistoryGunTab)
{
    ui->setupUi(this);
    fire_arc_scene = new FiringArcScene(ui->graphicsView);
    fire_arc_scene->setCurrentGun(gun_idx);
    ui->graphicsView->setScene(fire_arc_scene);
}

TurnHistoryGunTab::~TurnHistoryGunTab()
{
    delete ui;
}

void TurnHistoryGunTab::updateSelectedTurnReport(const QModelIndex &gun_turn_idx)
{
    QModelIndex crew_turn_idx = gun_turn_idx.parent();
    int action_taken_id = crew_turn_idx.siblingAtColumn(TurnCrewItem::Turn_Crew_Action_Taken).data().toInt();
    QVariant action_extra_data = crew_turn_idx.siblingAtColumn(TurnCrewItem::Turn_Crew_Action_Extra_Data).data();
    ui->shots_fired_lbl->setText(QString("%1").arg(action_taken_id == PlaneCrewItem::Action_Shoot ? action_extra_data.value<PlaneCrewItem::ShotProperties>().burst_len : 0));

    int current_box_ammo = gun_turn_idx.siblingAtColumn(TurnArmamentItem::Turn_Crew_Armament_Current_Box_Ammo).data().toInt();
    int ammo_box_capacity = gun_turn_idx.siblingAtColumn(TurnArmamentItem::Turn_Crew_Armament_Idx).data().toModelIndex()
                                .siblingAtColumn(PlaneArmamentsItem::Plane_Armaments_Ammo_Box_Capacity).data().toInt();
    int remaining_ammo_boxes = gun_turn_idx.siblingAtColumn(TurnArmamentItem::Turn_Crew_Armament_Remaining_Ammo_Boxes).data().toInt();
    int total_ammo = gun_turn_idx.siblingAtColumn(TurnArmamentItem::Turn_Crew_Armament_Idx).data().toModelIndex()
                         .siblingAtColumn(PlaneArmamentsItem::Plane_Armaments_Total_Ammo).data().toInt();

    ui->ammo_in_clip_lbl->setText(QString("%1/%2").arg(current_box_ammo).arg(ammo_box_capacity));
    ui->total_ammo_lbl->setText(QString("%1/%2").arg(remaining_ammo_boxes*ammo_box_capacity+current_box_ammo).arg(total_ammo));
    ui->jamCheckLbl->setText(gun_turn_idx.siblingAtColumn(TurnArmamentItem::Turn_Crew_Armament_IsJammed).data().toBool() ? "Yes" : "No");
    ui->gun_rotation_lbl->setText(gun_turn_idx.siblingAtColumn(TurnArmamentItem::Turn_Crew_Armament_Position).data().toString());
    fire_arc_scene->setGunRotation(gun_turn_idx.siblingAtColumn(TurnArmamentItem::Turn_Crew_Armament_Position).data().toInt());
}
