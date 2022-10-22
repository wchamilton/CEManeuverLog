#include "PlaneItems.h"
#include <QJsonArray>
#include <QDebug>

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
    setData(Plane_Name,            plane["name"].toVariant());
    setData(Plane_Era,             plane["plane_era"].toVariant());
    setData(Fuel,                  plane["fuel"].toVariant());
    setData(Engine_HP,             plane["engine_hp"].toVariant());
    setData(Engine_Critical,       plane["engine_critical"].toVariant());
    setData(Wing_HP,               plane["wing_hp"].toVariant());
    setData(Wing_Critical,         plane["wing_critical"].toVariant());
    setData(Fuselage_HP,           plane["fuselage_hp"].toVariant());
    setData(Fuselage_Critical,     plane["fuselage_critical"].toVariant());
    setData(Tail_HP,               plane["tail_hp"].toVariant());
    setData(Tail_Critical,         plane["tail_critical"].toVariant());
    setData(Rated_Climb,           plane["rated_climb"].toVariant());
    setData(Rated_Dive,            plane["rated_dive"].toVariant());
    setData(Max_Altitude,          plane["max_alt"].toString().left(1).toInt());
    setData(Can_Return_To_Max_Alt, plane["max_alt"].toString().right(1) == "+");
    setData(Stability,             plane["stability"].toVariant());
    setData(Is_Gliding,            false);
    setData(Is_On_Fire,            false);

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
        if (childAt(i)->getType() == Maneuver_Item_Type && childAt(i)->data(ManeuverItem::Added_To_Schedule).toBool()) {
            maneuvers << static_cast<ManeuverItem*>(childAt(i))->toJSON();
        }
        else if (childAt(i)->getType() == Crew_Item_Type) {
            crew << static_cast<CrewItem*>(childAt(i))->toJSON();
        }
    }

    plane["maneuvers"] = maneuvers;
    plane["crew"] = crew;
    return plane;
}

ManeuverItem::ManeuverItem(QJsonObject maneuver, BaseItem *parent) : BaseItem(Maneuver_Item_Type, parent)
{
    QString name = maneuver["name"].toString();
    setData(Maneuver_Name,        name);
    setData(IsEnabled,            true); // By default all available maneuvers are enabled
    setData(Can_Be_Used,          true); // This needs to be set dynamically turn by turn
    setData(Speed,                name.right(1).toInt());
    setData(Direction,            name.right(2).left(1));
    QStringList tolerances =      maneuver["tolerances"].toString().split("/");
    setData(Climb_Value,          tolerances.at(0));
    setData(Level_Value,          tolerances.at(1));
    setData(Dive_Value,           tolerances.at(2));
    setData(Can_Be_Repeated,      maneuver["can_be_repeated"].toVariant());
    setData(Can_Reload,           maneuver["can_reload"].toVariant());
    setData(Can_Put_Out_Fires,    maneuver["can_put_out_fires"].toVariant());
    setData(Observer_Can_Reload,  maneuver["observer_can_reload"].toVariant());
    setData(Is_Restricted,        maneuver["is_restricted"].toVariant());
    setData(Is_Weight_Restricted, maneuver["is_weight_restricted"].toVariant());
    setData(Is_Climb_Restricted,  maneuver["is_climb_restricted"].toVariant());
    setData(Causes_Spin_Check,    maneuver["causes_spin_check"].toVariant());
}

ManeuverItem::ManeuverItem(Maneuver maneuver, BaseItem *parent) : BaseItem(Maneuver_Item_Type, parent)
{
    setData(Maneuver_Name,       maneuver.name);
    setData(Speed,               maneuver.name.right(1).toInt());
    setData(Direction,           maneuver.name.right(2).left(1));
    setData(IsEnabled,           true);
    setData(Can_Be_Repeated,     true);
    setData(Added_To_Schedule,   false); // Maneuvers need to be added to the schedule to be saved
    setData(Climb_Value,         maneuver.climb_value);
    setData(Level_Value,         maneuver.level_value);
    setData(Dive_Value,          maneuver.dive_value);
    setData(Is_Restricted,       maneuver.is_restricted);
    setData(Is_Climb_Restricted, maneuver.is_climb_restricted);
    setData(Causes_Spin_Check,   maneuver.causes_spin_check);
}

QJsonObject ManeuverItem::toJSON() const
{
    QJsonObject maneuver;
    QStringList tolerances = {data(Climb_Value).toString(), data(Level_Value).toString(), data(Dive_Value).toString()};
    maneuver["name"] =                 data(Maneuver_Name).toString();
    maneuver["tolerances"] =           tolerances.join("/");
    maneuver["can_be_repeated"] =      data(Can_Be_Repeated).toBool();
    maneuver["can_reload"] =           data(Can_Reload).toBool();
    maneuver["can_put_out_fires"] =    data(Can_Put_Out_Fires).toBool();
    maneuver["observer_can_reload"] =  data(Observer_Can_Reload).toBool();
    maneuver["is_restricted"] =        data(Is_Restricted).toBool();
    maneuver["is_weight_restricted"] = data(Is_Weight_Restricted).toBool();
    maneuver["is_climb_restricted"] =  data(Is_Climb_Restricted).toBool();
    maneuver["causes_spin_check"] =    data(Causes_Spin_Check).toBool();
    return maneuver;
}

CrewItem::CrewItem(QJsonObject crew, BaseItem *parent) : BaseItem(Crew_Item_Type, parent)
{
    setData(Crew_Role, crew["role"].toString());
    setData(Can_Drop_Bombs, crew["can_drop_bombs"].toBool());
    setData(Wounds, 0);
    QJsonArray guns = crew.value("guns").toArray();

    for (int i=0; i<guns.size(); ++i) {
        addChild(new GunItem(guns.at(i).toObject(), this));
    }
}

CrewItem::CrewItem(BaseItem *parent) : BaseItem(BaseItem::Crew_Item_Type, parent) {}

QJsonObject CrewItem::toJSON() const
{
    QJsonObject crew;
    crew["role"] = data(Crew_Role).toString();
    crew["can_drop_bombs"] = data(Can_Drop_Bombs).toBool();

    QJsonArray guns;
    for (int i=0; i<childCount(); ++i) {
        guns << static_cast<GunItem*>(childAt(i))->toJSON();
    }
    crew["guns"] = guns;
    return crew;
}

GunItem::GunItem(QJsonObject gun, BaseItem *parent) : BaseItem(Gun_Item_Type, parent)
{
    setData(Gun_Name,          gun["name"].toVariant());
    setData(Gun_Count,         gun["gun_links"].toVariant());
    setData(Fire_Template,     gun["fire_template"].toVariant());
    setData(Fire_Base_3,       gun["fire_base_3"].toVariant());
    setData(Fire_Base_2,       gun["fire_base_2"].toVariant());
    setData(Fire_Base_1,       gun["fire_base_1"].toVariant());
    setData(Fire_Base_0,       gun["fire_base_0"].toVariant());
    setData(Ammo_Box_Capacity, gun["ammo_box_capacity"].toVariant());
    setData(Ammo_Box_Count,    gun["ammo_box_count"].toVariant());
    setData(Gun_Destroyed,     false);
}

GunItem::GunItem(BaseItem *parent) : BaseItem(BaseItem::Gun_Item_Type, parent) {}

QJsonObject GunItem::toJSON() const
{
    QJsonObject gun;
    gun["name"]              = data(Gun_Name).toString();
    gun["gun_links"]         = data(Gun_Count).toInt();
    gun["fire_template"]     = data(Fire_Template).toInt();
    gun["fire_base_3"]       = data(Fire_Base_3).toInt();
    gun["fire_base_2"]       = data(Fire_Base_2).toInt();
    gun["fire_base_1"]       = data(Fire_Base_1).toInt();
    gun["fire_base_0"]       = data(Fire_Base_0).toInt();
    gun["ammo_box_capacity"] = data(Ammo_Box_Capacity).toInt();
    gun["ammo_box_count"]    = data(Ammo_Box_Count).toInt();
    return gun;
}
