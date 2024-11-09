#include "GameModelItems.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QPersistentModelIndex>

PlaneItem::PlaneItem(QJsonObject plane_json, BaseItem* parent) : BaseItem(ItemType::Plane_Item_Type, parent)
{
    setData(Plane_Model,                    plane_json["name"].toVariant());
    setData(Plane_History,                  plane_json["history"].toVariant());
    setData(Plane_Era,                      plane_json["plane_era"].toVariant());
    setData(Plane_Fuel_Cap,                 plane_json["fuel"].toVariant());
    setData(Plane_Engine_HP,                plane_json["engine_hp"].toVariant());
    setData(Plane_Engine_Critical,          plane_json["engine_critical"].toVariant());
    setData(Plane_Wing_HP,                  plane_json["wing_hp"].toVariant());
    setData(Plane_Wing_Critical,            plane_json["wing_critical"].toVariant());
    setData(Plane_Fuselage_HP,              plane_json["fuselage_hp"].toVariant());
    setData(Plane_Fuselage_Critical,        plane_json["fuselage_critical"].toVariant());
    setData(Plane_Tail_HP,                  plane_json["tail_hp"].toVariant());
    setData(Plane_Tail_Critical,            plane_json["tail_critical"].toVariant());
    setData(Plane_Rated_Climb,              plane_json["rated_climb"].toVariant());
    setData(Plane_Rated_Dive,               plane_json["rated_dive"].toVariant());
    setData(Plane_Max_Altitude,             plane_json["max_alt"].toString().remove("+").toInt());
    setData(Plane_Can_Return_To_Max_Alt,    plane_json["max_alt"].toString().right(1) == "+");
    setData(Plane_Stability_Rating,         plane_json["stability"].toVariant());

    QJsonArray crew_roles = plane_json["crew"].toArray();
    QList<int> roles_list;
    for (int i=0; i<crew_roles.count(); ++i){
        roles_list << crew_roles.at(i).toInt();
    }
    setData(Plane_Crew_Roles, QVariant::fromValue(roles_list));
}

PlaneManeuverItem::PlaneManeuverItem(QJsonObject plane_maneuver_json, QPersistentModelIndex maneuver_index, BaseItem *parent) : BaseItem(ItemType::Plane_Maneuver_Item_Type, parent)
{
    setData(Plane_Maneuver_Index,                   maneuver_index);
    setData(Plane_Maneuver_Tolerances,              plane_maneuver_json["tolerances"].toVariant());
    setData(Plane_Maneuver_Is_Weight_Restricted,    plane_maneuver_json["is_weight_restricted"].toVariant());
}

PlaneArmamentsItem::PlaneArmamentsItem(QJsonObject plane_armaments_json, BaseItem *parent) : BaseItem(ItemType::Plane_Armaments_Item_Type, parent)
{
    setData(Plane_Armaments_Name,               plane_armaments_json["name"].toVariant());
    setData(Plane_Armaments_Fire_Template,      plane_armaments_json["fire_template"].toVariant());
    setData(Plane_Armaments_Fire_Base_3,        plane_armaments_json["fire_base_3"].toVariant());
    setData(Plane_Armaments_Fire_Base_2,        plane_armaments_json["fire_base_2"].toVariant());
    setData(Plane_Armaments_Fire_Base_1,        plane_armaments_json["fire_base_1"].toVariant());
    setData(Plane_Armaments_Fire_Base_0,        plane_armaments_json["fire_base_0"].toVariant());
    setData(Plane_Armaments_Ammo_Box_Capacity,  plane_armaments_json["ammo_box_capacity"].toVariant());
    setData(Plane_Armaments_Ammo_Box_Count,     plane_armaments_json["ammo_box_count"].toVariant());
}
