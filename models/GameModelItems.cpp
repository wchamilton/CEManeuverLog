#include "GameModelItems.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QPersistentModelIndex>

PlaneItem::PlaneItem(QJsonObject plane_json, BaseItem* parent) : BaseItem(ItemType::Plane_Item_Type, parent)
{
    setData(Plane_Name,                    plane_json["name"].toVariant());
    QString era_str = plane_json["plane_era"].toString();
    PlaneEra era = era_str == "Early War" ? PlaneEra::Era_Early_War :
                       era_str == "Late War" ? PlaneEra::Era_Late_War : PlaneEra::Era_UNKNOWN;
    setData(Plane_Era,                      era);
    setData(Plane_Points,                   plane_json["points"].toVariant());
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

QJsonObject PlaneItem::toJSON()
{
    QJsonObject json;
    json["name"] =              data(Plane_Name).toString();
    json["plane_era"] =         data(Plane_Era).toInt() == PlaneEra::Era_Early_War ? "Early War" :
                                    data(Plane_Era).toInt() == PlaneEra::Era_Late_War ? "Late War" : " UNKNOWN";
    json["points"] =            data(Plane_Points).toInt();
    json["fuel"] =              data(Plane_Fuel_Cap).toInt();
    json["engine_hp"] =         data(Plane_Engine_HP).toInt();
    json["engine_critical"] =   data(Plane_Engine_Critical).toInt();
    json["wing_hp"] =           data(Plane_Wing_HP).toInt();
    json["wing_critical"] =     data(Plane_Wing_Critical).toInt();
    json["fuselage_hp"] =       data(Plane_Fuselage_HP).toInt();
    json["fuselage_critical"] = data(Plane_Fuselage_Critical).toInt();
    json["tail_hp"] =           data(Plane_Tail_HP).toInt();
    json["tail_critical"] =     data(Plane_Tail_Critical).toInt();
    json["rated_dive"] =        data(Plane_Rated_Dive).toInt();
    json["rated_climb"] =       data(Plane_Rated_Climb).toInt();
    json["max_alt"] =           data(Plane_Max_Altitude).toString();
    json["stability"] =         data(Plane_Stability_Rating).toString();

    QJsonArray maneuver_array;
    QJsonArray crew_array;
    for (int i=0; i<childCount(); ++i) {
        if (childAt(i)->getType() == BaseItem::Plane_Maneuver_Item_Type) {
            maneuver_array << childAt(i)->toJSON();
        }
        else if (childAt(i)->getType() == BaseItem::Plane_Crew_Item_Type) {
            crew_array << childAt(i)->toJSON();
        }
        else {
            qWarning() << "An unknown child of the plane object attempted to return a json object";
        }
    }

    json["maneuvers"] = maneuver_array;
    json["crew"] = crew_array;

    return json;
}

PlaneManeuverItem::PlaneManeuverItem(Maneuver maneuver, BaseItem *parent) : BaseItem(ItemType::Plane_Maneuver_Item_Type, parent)
{
    QStringList tolerances = maneuver.tolerances.split('/');

    setData(Plane_Maneuver_Name,                    maneuver.name);
    setData(Plane_Maneuver_Speed,                   maneuver.name.right(1).toInt());
    setData(Plane_Maneuver_Tolerances,              tolerances);
    setData(Plane_Maneuver_Climb_Val,               tolerances.at(0));
    setData(Plane_Maneuver_Level_Val,               tolerances.at(1));
    setData(Plane_Maneuver_Dive_Val,                tolerances.at(2));
    setData(Plane_Maneuver_Is_Restricted,           maneuver.is_restricted);
    setData(Plane_Maneuver_Has_Climb_Condition,     maneuver.has_climb_condition);
    setData(Plane_Maneuver_Force_Spin_Check,        maneuver.causes_spin_check);
    setData(Plane_Maneuver_Tile_Movements,          QVariant::fromValue(maneuver.tile_movements));
    setData(Plane_Maneuver_Final_Rotation,          maneuver.final_rotation);
    setData(Plane_Maneuver_Can_Be_Repeated,         maneuver.name.right(1).toInt() != 0);
}

PlaneManeuverItem::PlaneManeuverItem(QJsonObject plane_maneuver_json, BaseItem *parent) : BaseItem(ItemType::Plane_Maneuver_Item_Type, parent)
{
    QString name =                                  plane_maneuver_json["name"].toString();
    QStringList tolerances =                        plane_maneuver_json["tolerances"].toString().split('/');
    setData(Plane_Maneuver_Name,                    name);
    setData(Plane_Maneuver_Speed,                   name.right(1).toInt());
    setData(Plane_Maneuver_Direction,               name.right(2).at(0));
    setData(Plane_Maneuver_Tolerances,              tolerances);
    setData(Plane_Maneuver_Climb_Val,               tolerances.at(0));
    setData(Plane_Maneuver_Level_Val,               tolerances.at(1));
    setData(Plane_Maneuver_Dive_Val,                tolerances.at(2));
    setData(Plane_Maneuver_Can_Reload,              plane_maneuver_json["can_reload"].toVariant());
    setData(Plane_Maneuver_Observer_Can_Reload,     plane_maneuver_json["observer_can_reload"].toVariant());
    setData(Plane_Maneuver_Put_Out_Fires_Bonus,     plane_maneuver_json["has_put_out_fires_bonus"].toVariant());
    setData(Plane_Maneuver_Has_Climb_Condition,     plane_maneuver_json["has_climb_condition"].toVariant());
    setData(Plane_Maneuver_Has_Weight_Restriction,  plane_maneuver_json["has_weight_restriction"].toVariant());
    setData(Plane_Maneuver_Can_Be_Repeated,         plane_maneuver_json["can_be_repeated"].toVariant());
    setData(Plane_Maneuver_Force_Spin_Check,        master_maneuver_map[name].causes_spin_check);
    setData(Plane_Maneuver_Tile_Movements,          QVariant::fromValue(master_maneuver_map[name].tile_movements));
    setData(Plane_Maneuver_Final_Rotation,          master_maneuver_map[name].final_rotation);
    setData(Plane_Maneuver_Is_Restricted,           master_maneuver_map[name].is_restricted);
}

QJsonObject PlaneManeuverItem::toJSON()
{
    QJsonObject json;
    QStringList tolerances = {data(Plane_Maneuver_Climb_Val).toString(),
                              data(Plane_Maneuver_Level_Val).toString(),
                              data(Plane_Maneuver_Dive_Val).toString()};
    json["name"] =                      data(Plane_Maneuver_Name).toString();
    json["tolerances"] =                tolerances.join("/");
    json["can_be_repeated"] =           data(Plane_Maneuver_Can_Be_Repeated).toBool();
    json["can_reload"] =                data(Plane_Maneuver_Can_Reload).toBool();
    json["has_put_out_fires_bonus"] =   data(Plane_Maneuver_Put_Out_Fires_Bonus).toBool();
    json["observer_can_reload"] =       data(Plane_Maneuver_Observer_Can_Reload).toBool();
    json["is_weight_restricted"] =      data(Plane_Maneuver_Has_Weight_Restriction).toBool();
    json["is_climb_restricted"] =       data(Plane_Maneuver_Has_Climb_Condition).toBool();
    return json;
}

PlaneArmamentsItem::PlaneArmamentsItem(QJsonObject plane_armaments_json, BaseItem *parent) : BaseItem(ItemType::Plane_Armaments_Item_Type, parent)
{
    setData(Plane_Armaments_Name,                   plane_armaments_json["name"].toVariant());
    setData(Plane_Armaments_Gun_Destroyed,          false);
    setData(Plane_Armaments_Gun_Jammed,             false);
    setData(Plane_Armaments_Gun_Is_Linked,          plane_armaments_json["is_linked"].toVariant());

    int fire_template =                             plane_armaments_json["fire_template"].toInt();
    setData(Plane_Armaments_Fire_Template,          fire_template);
    setData(Plane_Armaments_Fire_Base_3,            plane_armaments_json["fire_base_3"].toVariant());
    setData(Plane_Armaments_Fire_Base_2,            plane_armaments_json["fire_base_2"].toVariant());
    setData(Plane_Armaments_Fire_Base_1,            plane_armaments_json["fire_base_1"].toVariant());
    setData(Plane_Armaments_Fire_Base_0,            plane_armaments_json["fire_base_0"].toVariant());

    int box_capacity =                              plane_armaments_json["ammo_box_capacity"].toInt();
    int box_count =                                 plane_armaments_json["ammo_box_count"].toInt();
    setData(Plane_Armaments_Ammo_Box_Capacity,      box_capacity);
    setData(Plane_Armaments_Ammo_In_Current_Box,    box_capacity);
    setData(Plane_Armaments_Spare_Ammo_Box_Count,   box_count - 1); // Simulate first box already loaded
    setData(Plane_Armaments_Total_Ammo,             box_capacity * box_count);

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
    if (rotation_range.contains(1)) {
        setData(Plane_Armaments_Gun_Rotation_Current_Pos, 1);
        setData(Plane_Armaments_Gun_Rotation_Last_Turn, 1);
    }
    else if (rotation_range.contains(4)) {
        setData(Plane_Armaments_Gun_Rotation_Current_Pos, 4);
        setData(Plane_Armaments_Gun_Rotation_Last_Turn, 4);
    }
    else {
        setData(Plane_Armaments_Gun_Rotation_Current_Pos, rotation_range.first());
        setData(Plane_Armaments_Gun_Rotation_Last_Turn, rotation_range.first());
    }
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
            break;
        }
        case Plane_Armaments_Total_Ammo_Remaining:
            return BaseItem::data(Plane_Armaments_Total_Ammo).toInt() - BaseItem::data(Plane_Armaments_Shots_Fired).toInt();
        default: break;
    }
    return BaseItem::data(column);
}

QJsonObject PlaneArmamentsItem::toJSON()
{
    QJsonObject json;
    json["name"]              = data(Plane_Armaments_Name).toString();
    json["is_linked"]         = data(Plane_Armaments_Gun_Is_Linked).toBool();
    json["fire_template"]     = data(Plane_Armaments_Fire_Template).toInt();
    json["fire_base_3"]       = data(Plane_Armaments_Fire_Base_3).toInt();
    json["fire_base_2"]       = data(Plane_Armaments_Fire_Base_2).toInt();
    json["fire_base_1"]       = data(Plane_Armaments_Fire_Base_1).toInt();
    json["fire_base_0"]       = data(Plane_Armaments_Fire_Base_0).toInt();
    json["ammo_box_capacity"] = data(Plane_Armaments_Ammo_Box_Capacity).toInt();
    json["ammo_box_count"]    = data(Plane_Armaments_Spare_Ammo_Box_Count).toInt();
    return json;
}

PlaneCrewItem::PlaneCrewItem(QJsonObject plane_crew_json, BaseItem *parent) : BaseItem(ItemType::Plane_Crew_Item_Type, parent)
{
    int role = 0;
    setData(Plane_Crew_Role, plane_crew_json["role"].toString());
    if (plane_crew_json.contains("role_id")) {
        setData(Plane_Crew_Role_ID, plane_crew_json["role_id"].toInt());
    }
    else {
        role = plane_crew_json["role_id"].toInt();
        if (plane_crew_json["role"].toString() == "Co-Pilot") {
            role = CoPilot;
        }
        else if (plane_crew_json["role"].toString() == "Observer") {
            role = Observer;
        }
        else if (plane_crew_json["role"].toString() == "Gunner") {
            role = Gunner;
        }
        setData(Plane_Crew_Role_ID, role);
    }

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

QJsonObject PlaneCrewItem::toJSON()
{
    QJsonObject json;
    json["role_id"] =           data(Plane_Crew_Role).toString();
    json["can_drop_bombs"] =    data(Plane_Crew_Can_Drop_Payloads).toBool();

    /// TODO: REPLACE THIS GARBAGE WITH THE CREW ONLY
    QJsonArray guns;
    for (int i=0; i<childCount(); ++i) {
        if (childAt(i)->getType() == BaseItem::Plane_Armaments_Item_Type) {
            for (int j=0; j<childAt(i)->childCount(); ++j) {
                guns << childAt(i)->childAt(j)->toJSON();
            }
        }
        else if (childAt(i)->getType() == BaseItem::Plane_Armaments_Item_Type) {
            guns << childAt(i)->toJSON();
        }
    }
    json["guns"] = guns;
    return json;
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
        for (const auto &gun : std::as_const(linked_guns)) {
            QString gun_name = gun->data(column).toString();
            multi_gun_count[gun_name] = multi_gun_count.contains(gun_name) ? multi_gun_count[gun_name] + 1 : 1;
        }

        for (auto [gun_name, gun_name_count] : multi_gun_count.asKeyValueRange()) {
            QString prefix;
            switch (gun_name_count) {
                case 1: break;
                case 2: prefix = "Twin "; break;
                case 3: prefix = "Triple "; break;
                case 4: prefix = "Quad "; break;
                case 5: prefix = "Penta "; break;
                case 6: prefix = "Hexa "; break;
                case 7: prefix = "Hepta "; break;
                case 8: prefix = "Octa "; break;
                case 9: prefix = "Nona "; break;
                case 10: prefix = "Deca "; break; // How this ever happens, I don't know
                default: {
                    prefix = "Too Many ";
                    break;
                }
            }
            compound_name_components << prefix + gun_name;
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
        for (auto gun : std::as_const(linked_guns)) {
            // If any guns are drum based, base the other columns off it as that will be the most restrictive
            if (gun->data(PlaneArmamentsItem::Plane_Armaments_Spare_Ammo_Box_Count).toInt() > 1) {
                return gun->data(column);
            }
        }
        // Possible that they're all belt fed so just use the first one for all fields
        return linked_guns.first()->data(column);
    }
    }
}

QJsonObject GameItem::toJSON()
{
    QJsonObject json;
    json["player_name"] = data(Game_Player).toString();
    json["plane"] = static_cast<PlaneItem*>(data(Game_Plane_Selected).toPersistentModelIndex().internalPointer())->toJSON();
    json["conflict_name"] = data(Game_Conflict_Name).toString();
    json["conflict_date"] = data(Game_Conflict_Date).toDate().toString(Qt::ISODate);
    return json;
}

QJsonObject TurnItem::toJSON()
{
    QJsonObject json;
    json["turn_number"] = data(Turn_Number).toInt();
    json["selected_maneuver"] = data(Turn_Selected_Maneuver_Idx).toPersistentModelIndex().data().toString(); // The stored index will always be the name for convenience
    json["current_alt"] = data(Turn_Plane_Alt).toInt();
    json["speed_last_turn"] = data(Turn_Plane_Speed_Last_Turn).toInt();
    json["alt_last_turn"] = data(Turn_Plane_Alt_Last_Turn).toInt();
    json["current_fuel"] = data(Turn_Plane_Fuel).toInt();
    json["current_engine_hp"] = data(Turn_Plane_Engine_HP).toInt();
    json["current_wing_hp"] = data(Turn_Plane_Wing_HP).toInt();
    json["current_fuse_hp"] = data(Turn_Plane_Fuse_HP).toInt();
    json["current_tail_hp"] = data(Turn_Plane_Tail_HP).toInt();
    json["current_payload_count"] = data(Turn_Plane_Payload_Count).toInt();

    QJsonArray effects_array;
    QJsonArray crew_array;
    for (int i=0; i<childCount(); ++i) {
        if (childAt(i)->getType() == BaseItem::Turn_Effects_Type) {
            effects_array << childAt(i)->toJSON();
        }
        else if (childAt(i)->getType() == BaseItem::Turn_Crew_Item_Type) {
            crew_array << childAt(i)->toJSON();
        }
    }

    json["effects"] = effects_array;
    json["crew"] = crew_array;

    return json;
}

QJsonObject TurnPlaneEffects::toJSON()
{
    QJsonObject json;
    json["effect_id"] = data(Turn_Plane_Effect_ID).toInt();
    json["effect_name"] = data(Turn_Plane_Effect_Name).toString();
    json["remaining_turns"] = data(Turn_Plane_Effect_Remaining_Turns).toInt();
    return json;
}

QJsonObject TurnCrewItem::toJSON()
{
    QJsonObject json;
    json["crew_name"] = data(Turn_Crew_Idx).toPersistentModelIndex().data().toString(); // The stored index will always be the name for convenience
    json["action_taken"] = data(Turn_Crew_Action_Taken).toInt();
    // TODO: Handle extra data
    QVariant extra_data = data(Turn_Crew_Action_Extra_Data);
    switch(data(Turn_Crew_Action_Taken).toInt()) {
    case PlaneCrewItem::Action_None: json["action_extra_data"] = extra_data.toString(); break;
    case PlaneCrewItem::Action_Shoot: json["action_extra_data"] = extra_data.value<PlaneCrewItem::ShotProperties>().toJson(); break;
    case PlaneCrewItem::Action_Reload: json["action_extra_data"] = extra_data.toString(); break;
    case PlaneCrewItem::Action_Unjam: json["action_extra_data"] = extra_data.toBool(); break;
    case PlaneCrewItem::Action_Drop_Payload: json["action_extra_data"] = extra_data.toBool(); break;
    case PlaneCrewItem::Action_Observe: json["action_extra_data"] = extra_data.toString(); break;
    case PlaneCrewItem::Action_Custom: json["action_extra_data"] = extra_data.toString(); break;
    default: break;
    }
    json["wounds"] = data(Turn_Crew_Wounds_Accrued).toInt();
    json["reds"] = data(Turn_Crew_Total_Red_Hits).toInt();
    json["kills"] = data(Turn_Crew_Total_Kills).toInt();

    QJsonArray crew_weapon_array;
    for (int i=0; i<childCount(); ++i) {
        crew_weapon_array << childAt(i)->toJSON();
    }
    json["guns"] = crew_weapon_array;

    return json;
}

QJsonObject TurnArmamentItem::toJSON()
{
    QJsonObject json;
    json["gun_name"] = data(Turn_Crew_Armament_Idx).toPersistentModelIndex().data().toString();
    json["rotation_pos"] = data(Turn_Crew_Armament_Position).toInt();
    json["is_destroyed"] = data(Turn_Crew_Armament_Is_Destroyed).toBool();
    json["is_jammed"] = data(Turn_Crew_Armament_IsJammed).toBool();
    json["current_box_ammo"] = data(Turn_Crew_Armament_Current_Box_Ammo).toInt();
    json["remaining_ammo_boxes"] = data(Turn_Crew_Armament_Remaining_Ammo_Boxes).toInt();
    return json;
}
