#include "PlaneItems.h"
#include <QJsonArray>

QVariant BaseItem::data(int column) const
{
    if (column >= 0 && column < column_data.size()) {
        return column_data.value(column);
    }
    return QVariant();
}

void BaseItem::setData(int column, QVariant data)
{
    if (column >= 0) {
        column_data[column] = data;
    }
}

PlaneItem::PlaneItem(QJsonObject plane, BaseItem *parent) : BaseItem(parent)
{
    setData(Plane_Name,        plane.value("name").toString());
    setData(Plane_Era,         plane.value("plane_era").toString());
    setData(Fuel,              plane.value("fuel").toInt());
    setData(Engine_HP,         plane.value("engine_hp").toInt());
    setData(Engine_Critical,   plane.value("engine_critical").toInt());
    setData(Wing_HP,           plane.value("wing_hp").toInt());
    setData(Wing_Critical,     plane.value("wing_critical").toInt());
    setData(Fuselage_HP,       plane.value("fuselage_hp").toInt());
    setData(Fuselage_Critical, plane.value("fuselage_critical").toInt());
    setData(Tail_HP,           plane.value("tail_hp").toInt());
    setData(Tail_Critical,     plane.value("tail_critical").toInt());
    setData(Dive,              plane.value("dive").toInt());
    setData(Climb,             plane.value("climb").toInt());
    setData(Altitude,          plane.value("alt").toString());
    setData(Stability,         plane.value("stability").toString());
    setData(Is_On_Fire,        false);

    QJsonArray maneuvers = plane.value("maneuvers").toArray();
    for (int i=0; i<maneuvers.size(); ++i) {
        addChild(new ManeuverItem(maneuvers.at(i).toObject(), this));
    }

    QJsonArray crew_members = plane.value("crew").toArray();
    for (int i=0; i<crew_members.size(); ++i) {
        addChild(new CrewItem(crew_members.at(i).toObject(), this));
    }
}

ManeuverItem::ManeuverItem(QJsonObject maneuver, BaseItem *parent) : BaseItem(parent)
{
    QString name = maneuver.value("name").toString();
    setData(Maneuver_Name,       name);
    setData(Speed,               name.right(1).toInt());
    setData(Direction,           name.right(2).left(1));
    setData(Climb_Value,         maneuver.value("climb_val").toString());
    setData(Level_Value,         maneuver.value("level_val").toString());
    setData(Dive_Value,          maneuver.value("dive_val").toString());
    setData(Can_Reload,          maneuver.contains("can_reload"));
    setData(Observer_Can_Reload, maneuver.contains("observer_can_reload"));
    setData(Is_Restricted,       maneuver.contains("is_restricted"));
    setData(Climb_Restricted,    maneuver.contains("climb_restricted"));
    setData(Can_Put_Out_Fires,   maneuver.contains("can_put_out_fires"));
    setData(Is_Bomb_Restricted,  maneuver.contains("is_bomb_restricted"));
}

CrewItem::CrewItem(QJsonObject crew, BaseItem *parent) : BaseItem(parent)
{
    // TODO: Add container/controls for special abilities
    setData(Crew_Role,     crew.value("role").toString());
    setData(Ammo_Box_Size, crew.value("gun_box_size").toInt());

    int ammo_count = crew.value("gun_ammo_amt").toInt();
    bool has_gun = ammo_count > 0;
    setData(Ammo_Amount,   ammo_count);
    setData(Has_Gun,       has_gun);
    setData(Fire_Base_3,   has_gun ? crew.value("fire_base_3").toInt() : 0);
    setData(Fire_Base_2,   has_gun ? crew.value("fire_base_2").toInt() : 0);
    setData(Fire_Base_1,   has_gun ? crew.value("fire_base_1").toInt() : 0);
    setData(Fire_Base_0,   has_gun ? crew.value("fire_base_0").toInt() : 0);
    setData(Gun_Destroyed, false);
}
