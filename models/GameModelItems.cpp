#include "GameModelItems.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QPersistentModelIndex>

PlaneItem::PlaneItem(QJsonObject plane_json, BaseItem* parent) : BaseItem(ItemType::Plane_Item_Type, parent)
{
    setData(Plane_Model,                    plane_json["name"].toVariant());
    QString era_str = plane_json["plane_era"].toString();
    PlaneEra era = era_str == "Early War" ? PlaneEra::Era_Early_War :
                       era_str == "Late War" ? PlaneEra::Era_Late_War : PlaneEra::Era_UNKNOWN;
    setData(Plane_Era,                      era);
    // setData(Plane_Points,                   plane_json["points"].toVariant());
    setData(Plane_Current_Fuel,             plane_json["fuel"].toVariant());
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

    QJsonArray maneuvers = plane_json.value("maneuvers").toArray();
    for (int i=0; i<maneuvers.size(); ++i) {
        addChild(new PlaneManeuverItem(maneuvers.at(i).toObject(), this));
    }

    QJsonArray crew = plane_json.value("crew").toArray();
    for (int i=0; i<crew.size(); ++i){
        addChild(new PlaneCrewItem(crew.at(i).toObject(), this));
    }
}

PlaneManeuverItem::PlaneManeuverItem(Maneuver maneuver, BaseItem *parent) : BaseItem(ItemType::Plane_Maneuver_Item_Type, parent)
{
    QStringList tolerances = maneuver.tolerances.split('/');

    setData(Plane_Maneuver_Name,                    maneuver.name);
    setData(Plane_Maneuver_Tolerances,              tolerances);
    setData(Plane_Maneuver_Climb_Val,               tolerances.at(0));
    setData(Plane_Maneuver_Level_Val,               tolerances.at(1));
    setData(Plane_Maneuver_Dive_Val,                tolerances.at(2));
    setData(Plane_Maneuver_Is_Restricted,           maneuver.is_restricted);
    setData(Plane_Maneuver_Has_Climb_Condition,     maneuver.has_climb_condition);
    setData(Plane_Maneuver_Force_Spin_Check,        maneuver.causes_spin_check);
    setData(Plane_Maneuver_Tile_Movements,          QVariant::fromValue(maneuver.tile_movements));
    setData(Plane_Maneuver_Final_Rotation,          maneuver.final_rotation);
}

PlaneManeuverItem::PlaneManeuverItem(QJsonObject plane_maneuver_json, BaseItem *parent) : BaseItem(ItemType::Plane_Maneuver_Item_Type, parent)
{
    QString name =                                  plane_maneuver_json["name"].toString();
    QStringList tolerances =                        plane_maneuver_json["tolerances"].toString().split('/');
    setData(Plane_Maneuver_Name,                    name);
    setData(Plane_Maneuver_Speed,                   name.right(1).toInt());
    setData(Plane_Maneuver_Direction,               name.right(2).left(1));
    setData(Plane_Maneuver_Tolerances,              tolerances);
    setData(Plane_Maneuver_Climb_Val,               tolerances.at(0));
    setData(Plane_Maneuver_Level_Val,               tolerances.at(1));
    setData(Plane_Maneuver_Dive_Val,                tolerances.at(2));
    setData(Plane_Maneuver_Can_Reload,              plane_maneuver_json["can_reload"].toVariant());
    setData(Plane_Maneuver_Observer_Can_Reload,     plane_maneuver_json["observer_can_reload"].toVariant());
    setData(Plane_Maneuver_Put_Out_Fires_Bonus,     plane_maneuver_json["can_put_out_fires"].toVariant());
    setData(Plane_Maneuver_Has_Climb_Condition,     plane_maneuver_json["has_climb_condition"].toVariant());
    setData(Plane_Maneuver_Has_Weight_Restriction,  plane_maneuver_json["has_weight_restriction"].toVariant());
    setData(Plane_Maneuver_Can_Be_Repeated,         plane_maneuver_json["can_be_repeated"].toVariant());
    setData(Plane_Maneuver_Force_Spin_Check,        master_maneuver_map[name].causes_spin_check);
    setData(Plane_Maneuver_Tile_Movements,          QVariant::fromValue(master_maneuver_map[name].tile_movements));
    setData(Plane_Maneuver_Final_Rotation,          master_maneuver_map[name].final_rotation);
}

PlaneArmamentsItem::PlaneArmamentsItem(QJsonObject plane_armaments_json, BaseItem *parent) : BaseItem(ItemType::Plane_Armaments_Item_Type, parent)
{
    setData(Plane_Armaments_Name,               plane_armaments_json["name"].toVariant());
    setData(Plane_Armaments_Gun_Destroyed,      false); // Need to track this individually in cases of linked weapons
    setData(Plane_Armaments_Gun_Is_Linked,      plane_armaments_json["is_linked"].toVariant());

    int fire_template =                         plane_armaments_json["fire_template"].toInt();
    setData(Plane_Armaments_Fire_Template,      fire_template);
    setData(Plane_Armaments_Fire_Base_3,        plane_armaments_json["fire_base_3"].toVariant());
    setData(Plane_Armaments_Fire_Base_2,        plane_armaments_json["fire_base_2"].toVariant());
    setData(Plane_Armaments_Fire_Base_1,        plane_armaments_json["fire_base_1"].toVariant());
    setData(Plane_Armaments_Fire_Base_0,        plane_armaments_json["fire_base_0"].toVariant());

    int box_capacity =                          plane_armaments_json["ammo_box_capacity"].toInt();
    int box_count =                             plane_armaments_json["ammo_box_count"].toInt();
    setData(Plane_Armaments_Ammo_Box_Capacity,  box_capacity);
    setData(Plane_Armaments_Ammo_Box_Count,     box_count);
    setData(Plane_Armaments_Total_Ammo,         box_capacity * box_count);

    QList<int> rotation_range;
    switch (fire_template) {
    case 1:  rotation_range = {1}; break;
    case 2:  rotation_range = {3,4,5}; break;
    case 3:
    case 4:  rotation_range = {1,2,3,4,5,6}; break;
    case 5:
    case 6:  rotation_range = {1,2,6}; break;
    case 7:  rotation_range = {1,2,6}; break;
    case 8:  rotation_range = {3,4,5}; break;
    case 9:  rotation_range = {4}; break;
    case 10:
    case 11: rotation_range = {1,2,3,4,5,6}; break;
    case 12: rotation_range = {1}; break;
    case 13: rotation_range = {3,4,5}; break;
    case 14: rotation_range = {2,6}; break;
    case 15: rotation_range = {5}; break;
    case 16: rotation_range = {3}; break;
    case 17: rotation_range = {1,2,3,4,5,6}; break;
    }

    setData(Plane_Armaments_Gun_Rotation_Range, QVariant::fromValue(rotation_range));
}

QVariant PlaneArmamentsItem::data(int column) const
{
    switch (column) {
        case Plane_Armaments_Fire_Base_0:
        case Plane_Armaments_Fire_Base_1:
        case Plane_Armaments_Fire_Base_2:
        case Plane_Armaments_Fire_Base_3: {
            if (BaseItem::data(column).toInt() == 0) {
                return BaseItem::data(Plane_Armaments_Gun_Is_Linked).toBool() ? 0 : "-";
            }
        }
        default: break;
    }
    return BaseItem::data(column);
}

PlaneCrewItem::PlaneCrewItem(QJsonObject plane_crew_json, BaseItem *parent) : BaseItem(ItemType::Plane_Crew_Item_Type, parent)
{
    int role = 0;
    if (plane_crew_json.contains("role_id")) {
        role = plane_crew_json["role_id"].toInt();
    }
    else if (plane_crew_json["role"].toString() == "Co-Pilot") {
        role = CoPilot;
    }
    else if (plane_crew_json["role"].toString() == "Observer") {
        role = Observer;
    }
    else if (plane_crew_json["role"].toString() == "Gunner") {
        role = Gunner;
    }
    setData(Plane_Crew_Role,              role);
    setData(Plane_Crew_Can_Drop_Payloads, plane_crew_json["can_drop_bombs"].toBool());

    QJsonArray armaments = plane_crew_json.value("guns").toArray();
    PlaneArmamentLinkItem* link_item = nullptr; // Initialize this if we find linked guns
    for (int i=0; i<armaments.size(); ++i) {
        if (armaments.at(i).toObject().value("is_linked").toBool()) {
            if (link_item == nullptr) {
                link_item = new PlaneArmamentLinkItem(this);
                addChild(link_item);
            }
            link_item->addChild(new PlaneArmamentsItem(armaments.at(i).toObject(), link_item));
        }
        else {
            addChild(new PlaneArmamentsItem(armaments.at(i).toObject(), this));
        }
    }
}

QVariant PlaneArmamentLinkItem::data(int column) const
{
    QList<BaseItem*> linked_guns;
    for (int i=0; i<childCount(); ++i) {
        if (!childAt(i)->data(PlaneArmamentsItem::Plane_Armaments_Gun_Destroyed).toBool()) {
            linked_guns << childAt(i);
        }
    }
    if (linked_guns.isEmpty()) {
        return QVariant();
    }
    switch (column) {
    case PlaneArmamentsItem::Plane_Armaments_Name: {
        QStringList compound_name_components;
        QMap<QString, int> multi_gun_count;
        for (const auto &gun : linked_guns) {
            QString gun_name = gun->data(column).toString();
            multi_gun_count[gun_name] = multi_gun_count.contains(gun_name) ? multi_gun_count[gun_name] + 1 : 1;
        }

        for (QString gun_name : multi_gun_count.keys()) {
            switch (multi_gun_count[gun_name]) {
                case 2: gun_name.prepend("Twin "); break;
                case 3: gun_name.prepend("Triple "); break;
                case 4: gun_name.prepend("Quad "); break;
                case 5: gun_name.prepend("Penta ");
                case 6: gun_name.prepend("Hexa "); break;
            }
            compound_name_components << gun_name;
        }

        return compound_name_components.join(" + ");
    }
    case PlaneArmamentsItem::Plane_Armaments_Fire_Base_0:
    case PlaneArmamentsItem::Plane_Armaments_Fire_Base_1:
    case PlaneArmamentsItem::Plane_Armaments_Fire_Base_2:
    case PlaneArmamentsItem::Plane_Armaments_Fire_Base_3: {
        if (linked_guns.first()->data(column).toInt() == 0) {
            return "-";
        }
        return linked_guns.first()->data(column).toInt() + 2*(linked_guns.size()-1);
    }
    default: {
        for (auto gun : linked_guns) {
            // If any guns are drum based, base the other columns off it as that will be the most restrictive
            if (gun->data(PlaneArmamentsItem::Plane_Armaments_Ammo_Box_Count).toInt() > 1) {
                return gun->data(column);
            }
        }
        // Possible that they're all belt fed so just use the first one for all fields
        return linked_guns.first()->data(column);
    }
    }
}
