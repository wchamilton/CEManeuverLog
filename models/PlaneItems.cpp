#include "PlaneItems.h"
#include <QJsonArray>
#include <QDebug>

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
    setData(Is_Spinning,           false);
    setData(Rudder_State,          RudderStates::Rudder_Normal);
    setData(Rudder_Jam_Duration,   0);

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
    setData(Can_Be_Used,         true);
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
    /// TODO: Store this as an enumeration and display the correct role via a switch statement
    setData(Crew_Role, crew["role"].toString());
    setData(Can_Drop_Bombs, crew["can_drop_bombs"].toBool());
    setData(Wounds, 0);
    setData(Has_Unrestricted_Maneuvers, false);
    setData(Has_Ignore_Deflection, false);
    QJsonArray guns = crew.value("guns").toArray();

    GunLinkItem* link_item = nullptr; // Initialize this if we find linked guns

    // Iterate over the guns in the json, any linked items get stored under the same umbrella while others get separated
    for (int i=0; i<guns.size(); ++i) {
        if (guns.at(i).toObject().value("is_linked").toBool()) {
            if (link_item == nullptr) {
                link_item = new GunLinkItem(this);
                addChild(link_item);
            }
            link_item->addChild(new GunItem(guns.at(i).toObject(), link_item));
        }
        else {
            addChild(new GunItem(guns.at(i).toObject(), this));
        }
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
        if (childAt(i)->getType() == BaseItem::Gun_Link_Item_Type) {
            for (int j=0; j<childAt(i)->childCount(); ++j) {
                guns << static_cast<GunItem*>(childAt(i)->childAt(j))->toJSON();
            }
        }
        else if (childAt(i)->getType() == BaseItem::Gun_Item_Type) {
            guns << static_cast<GunItem*>(childAt(i))->toJSON();
        }
    }
    crew["guns"] = guns;
    return crew;
}

// Need a proxy for the gun item that will consolidate linked guns to display as one powerful one but requiring the more
// restrictive of the two
GunItem::GunItem(QJsonObject gun, BaseItem *parent) : BaseItem(Gun_Item_Type, parent)
{
    setData(Gun_Name,            gun["name"].toVariant());
    setData(Gun_Is_Linked,       gun["is_linked"].toBool());
    setData(Fire_Template,       gun["fire_template"].toVariant());
    setData(Fire_Base_3,         gun["fire_base_3"].toVariant());
    setData(Fire_Base_2,         gun["fire_base_2"].toVariant());
    setData(Fire_Base_1,         gun["fire_base_1"].toVariant());
    setData(Fire_Base_0,         gun["fire_base_0"].toVariant());
    setData(Shots_Fired,         0);
    setData(Ammo_Box_Capacity,   gun["ammo_box_capacity"].toVariant());
    setData(Ammo_Box_Count,      gun["ammo_box_count"].toVariant());
    setData(Ammo_In_Current_Box, gun["ammo_box_capacity"].toVariant());
    setData(Total_Ammo,          gun["ammo_box_capacity"].toInt() * gun["ammo_box_count"].toInt());
    setData(Gun_Destroyed,       false);

    int fire_template = data(Fire_Template).toInt();
    if (QList<int>{2,8,9,13}.contains(fire_template)) {
        setData(Gun_Position, 4);
    }
    else if (fire_template == 14) {
        setData(Gun_Position, 2);
    }
    else if (fire_template == 15 || fire_template == 16) {
        setData(Gun_Position, 3);
    }
    else {
        setData(Gun_Position, 1);
    }
    setData(Gun_Last_Position, data(Gun_Position));

    QList<int> rotation_range;
    switch (data(Fire_Template).toInt()) {
        case 1: rotation_range = {1}; break;
        case 2: rotation_range = {3,4,5}; break;
        case 3:
        case 4: rotation_range = {1,2,3,4,5,6}; break;
        case 5:
        case 6: rotation_range = {1,2,6}; break;
        case 7: rotation_range = {1,2,6}; break;
        case 8: rotation_range = {3,4,5}; break;
        case 9: rotation_range = {4}; break;
        case 10:
        case 11: rotation_range = {1,2,3,4,5,6}; break;
        case 12: rotation_range = {1}; break;
        case 13: rotation_range = {3,4,5}; break;
        case 14: rotation_range = {2,6}; break;
        case 15: rotation_range = {5}; break;
        case 16: rotation_range = {3}; break;
        case 17: rotation_range = {1,2,3,4,5,6}; break;
    }

    setData(Gun_Position_Range, QVariant::fromValue(rotation_range));
}

GunItem::GunItem(BaseItem *parent) : BaseItem(BaseItem::Gun_Item_Type, parent) {}

QVariant GunItem::data(int column) const
{
    if (column == Total_Ammo_Remaining) {
        return data(Ammo_Box_Capacity).toInt() * std::max(data(Ammo_Box_Count).toInt() - 1, 0) + data(Ammo_In_Current_Box).toInt();
    }
    return BaseItem::data(column);
}

QJsonObject GunItem::toJSON() const
{
    QJsonObject gun;
    gun["name"]              = data(Gun_Name).toString();
    gun["is_linked"]         = data(Gun_Is_Linked).toBool();
    gun["fire_template"]     = data(Fire_Template).toInt();
    gun["fire_base_3"]       = data(Fire_Base_3).toInt();
    gun["fire_base_2"]       = data(Fire_Base_2).toInt();
    gun["fire_base_1"]       = data(Fire_Base_1).toInt();
    gun["fire_base_0"]       = data(Fire_Base_0).toInt();
    gun["ammo_box_capacity"] = data(Ammo_Box_Capacity).toInt();
    gun["ammo_box_count"]    = data(Ammo_Box_Count).toInt();
    return gun;
}

GunLinkItem::GunLinkItem(BaseItem *parent) : BaseItem(BaseItem::Gun_Link_Item_Type, parent) {}

QVariant GunLinkItem::data(int column) const
{
    QList<BaseItem*> linked_guns;
    for (int i=0; i<childCount(); ++i) {
        if (!childAt(i)->data(GunItem::Gun_Destroyed).toBool()) {
            linked_guns << childAt(i);
        }
    }
    switch (column) {
        case GunItem::Gun_Name: {
            QStringList compound_name_components;
            for (auto gun : linked_guns) {
                compound_name_components << gun->data(column).toString();
            }
            return compound_name_components.join(" + ");
        }
        case GunItem::Fire_Base_0:
        case GunItem::Fire_Base_1:
        case GunItem::Fire_Base_2:
        case GunItem::Fire_Base_3: {
            return linked_guns.first()->data(column).toInt() + 2*(linked_guns.size()-1);
        }
        default: {
            for (auto gun : linked_guns) {
                // If any guns are drum based, base the other columns off it as that will be the most restrictive
                if (gun->data(GunItem::Ammo_Box_Count).toInt() > 1) {
                    return gun->data(column);
                }
            }
            // Possible that they're all belt fed so just use the first one for all fields
            return linked_guns.first()->data(column);
        }
    }
}
