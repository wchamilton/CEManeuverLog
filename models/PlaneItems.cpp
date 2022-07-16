#include "PlaneItems.h"
#include <QJsonArray>
#include <QDebug>

#include "CEManeuvers.h"

BaseItem::~BaseItem()
{
    qDeleteAll(children);
}

QVariant BaseItem::data(int column) const
{
    return column_data.value(column);
}

void BaseItem::setData(int column, QVariant data)
{
    column_data[column] = data;
}

PlaneItem::PlaneItem(QJsonObject plane, BaseItem *parent) : BaseItem(Plane_Item_Type, parent)
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
    setData(Rated_Dive,        plane.value("rated_dive").toInt());
    setData(Rated_Climb,       plane.value("rated_climb").toInt());
    setData(Max_Altitude,      plane.value("max_alt").toString());
    setData(Stability,         plane.value("stability").toString());
    setData(Is_Gliding,        false);
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

PlaneItem::PlaneItem(BaseItem *parent) : BaseItem(Plane_Item_Type, parent)
{
    for (auto maneuver : master_maneuver_list) {
        addChild(new ManeuverItem(maneuver, this));
    }
}

QJsonObject PlaneItem::toJSON() const
{
    QJsonObject plane;
    plane["name"] = data(Plane_Name).toString();
    plane["plane_era"] = data(Plane_Era).toString();
    plane["fuel"] = data(Fuel).toInt();
    plane["engine_hp"] = data(Engine_HP).toInt();
    plane["engine_critical"] = data(Engine_Critical).toInt();
    plane["wing_hp"] = data(Wing_HP).toInt();
    plane["wing_critical"] = data(Wing_Critical).toInt();
    plane["fuselage_hp"] = data(Fuselage_HP).toInt();
    plane["fuselage_critical"] = data(Fuselage_Critical).toInt();
    plane["tail_hp"] = data(Tail_HP).toInt();
    plane["tail_critical"] = data(Tail_Critical).toInt();
    plane["rated_dive"] = data(Rated_Dive).toInt();
    plane["rated_climb"] = data(Rated_Climb).toInt();
    plane["max_alt"] = data(Max_Altitude).toString();
    plane["stability"] = data(Stability).toString();

    QJsonArray maneuvers;
    QJsonArray crew;
    for (int i=0; i<childCount(); ++i) {
        if (childAt(i)->getType() == Maneuver_Item_Type) {
            maneuvers << static_cast<ManeuverItem*>(childAt(i))->toJSON();
        }
        else if (childAt(i)->getType() == Crew_Item_Type) {
            crew << static_cast<CrewItem*>(childAt(i))->toJSON();
        }
    }

    plane["maneuvers"] = maneuvers;
    plane["crew"] = crew;
}

ManeuverItem::ManeuverItem(QJsonObject maneuver, BaseItem *parent) : BaseItem(Maneuver_Item_Type, parent)
{
    QString name = maneuver.value("name").toString();
    setData(Maneuver_Name,        name);
    setData(Speed,                name.right(1).toInt());
    setData(Direction,            name.right(2).left(1));
    setData(Climb_Value,          maneuver.value("climb_val").toString());
    setData(Level_Value,          maneuver.value("level_val").toString());
    setData(Dive_Value,           maneuver.value("dive_val").toString());
    setData(IsEnabled,            true); // By default all available maneuvers are enabled
    setData(Can_Be_Used,          true); // This needs to be set dynamically turn by turn
    setData(Can_Reload,           maneuver.contains("can_reload"));
    setData(Observer_Can_Reload,  maneuver.contains("observer_can_reload"));
    setData(Is_Restricted,        maneuver.contains("is_restricted"));
    setData(Climb_Restricted,     maneuver.contains("climb_restricted"));
    setData(Can_Put_Out_Fires,    maneuver.contains("can_put_out_fires"));
    setData(Can_Be_Repeated,      maneuver.contains("can_be_repeated"));
    setData(Is_Weight_Restricted, maneuver.contains("is_weight_restricted"));
}

ManeuverItem::ManeuverItem(Maneuver maneuver, BaseItem *parent) : BaseItem(Maneuver_Item_Type, parent)
{
    setData(Maneuver_Name,     maneuver.name);
    setData(Speed,             maneuver.name.right(1).toInt());
    setData(Direction,         maneuver.name.right(2).left(1));
    setData(IsEnabled,         true);
    setData(Can_Be_Repeated,   true);
    setData(Climb_Value,       maneuver.climb_value);
    setData(Level_Value,       maneuver.level_value);
    setData(Dive_Value,        maneuver.dive_value);
    setData(Is_Restricted,     maneuver.is_restricted);
    setData(Climb_Restricted,  maneuver.is_climb_restricted);
    setData(Causes_Spin_Check, maneuver.causes_spin_check);
}

QJsonObject ManeuverItem::toJSON() const
{

}

CrewItem::CrewItem(QJsonObject crew, BaseItem *parent) : BaseItem(Crew_Item_Type, parent)
{
    // TODO: Add container/controls for special abilities
    setData(Crew_Role, crew.value("role").toString());
    setData(Wounds, 0);
}

CrewItem::CrewItem(BaseItem *parent) : BaseItem(BaseItem::Crew_Item_Type, parent) {}

QJsonObject CrewItem::toJSON() const
{

}

GunItem::GunItem(QJsonObject gun, BaseItem *parent) : BaseItem(Gun_Item_Type, parent)
{
    setData(Ammo_Box_Capacity, gun.value("gun_box_size").toInt());
    int ammo_count = gun.value("gun_ammo_amt").toInt();
    setData(Ammo_Box_Count,   ammo_count);
    setData(Fire_Template, gun.value("fire_template").toInt());
    setData(Fire_Base_3,   gun.value("fire_base_3").toInt());
    setData(Fire_Base_2,   gun.value("fire_base_2").toInt());
    setData(Fire_Base_1,   gun.value("fire_base_1").toInt());
    setData(Fire_Base_0,   gun.value("fire_base_0").toInt());
    setData(Gun_Destroyed, false);
}

GunItem::GunItem(BaseItem *parent) : BaseItem(BaseItem::Gun_Item_Type, parent) {}

QJsonObject GunItem::toJSON() const
{

}
