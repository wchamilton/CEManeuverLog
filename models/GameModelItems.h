#ifndef GAMEMODELITEMS_H
#define GAMEMODELITEMS_H

#include "CEManeuvers.h"

/**
 * @brief The PlaneItem class
 */
class PlaneItem : public BaseItem
{
public:
    enum PlaneItemCols {
        Plane_Model = 0,
        Plane_Era,
        Plane_Points,
        Plane_Fuel_Cap,
        Plane_Engine_HP,
        Plane_Engine_Critical,
        Plane_Wing_HP,
        Plane_Wing_Critical,
        Plane_Fuselage_HP,
        Plane_Fuselage_Critical,
        Plane_Tail_HP,
        Plane_Tail_Critical,
        Plane_Rated_Climb,
        Plane_Rated_Dive,
        Plane_Max_Altitude,
        Plane_Can_Return_To_Max_Alt,
        Plane_Stability_Rating
    };

    enum PlaneEra {
        Era_UNKNOWN = -1,
        Era_Early_War,
        Era_Late_War
    };

    PlaneItem(BaseItem* parent = nullptr) : BaseItem(ItemType::Plane_Item_Type, parent) {}
    PlaneItem(QJsonObject plane_json, BaseItem* parent = nullptr);
};

/**
 * @brief The PlaneManeuverItem class
 */
class PlaneManeuverItem : public BaseItem
{
public:
    enum PlaneManeuverItemCols {
        Plane_Maneuver_Name = 0,
        Plane_Maneuver_Speed,
        Plane_Maneuver_Direction,
        Plane_Maneuver_Tolerances,
        Plane_Maneuver_Climb_Val,
        Plane_Maneuver_Level_Val,
        Plane_Maneuver_Dive_Val,
        Plane_Maneuver_Can_Reload,
        Plane_Maneuver_Observer_Can_Reload,
        Plane_Maneuver_Put_Out_Fires_Bonus,
        Plane_Maneuver_Is_Restricted,
        Plane_Maneuver_Has_Climb_Condition,
        Plane_Maneuver_Has_Weight_Restriction,
        Plane_Maneuver_Can_Be_Repeated,
        Plane_Maneuver_Force_Spin_Check,
        Plane_Maneuver_Tile_Movements,
        Plane_Maneuver_Final_Rotation
    };

    PlaneManeuverItem(Maneuver maneuver, BaseItem* parent = nullptr);
    PlaneManeuverItem(QJsonObject plane_maneuver_json, BaseItem* parent = nullptr);
};

/**
 * @brief The PlaneArmamentsItem class
 */
class PlaneArmamentsItem : public BaseItem
{
public:
    enum PlaneArmamentsItemCols {
        Plane_Armaments_Name = 0,
        Plane_Armaments_Gun_Destroyed,
        Plane_Armaments_Gun_Is_Linked,
        Plane_Armaments_Fire_Template,
        Plane_Armaments_Fire_Base_3,
        Plane_Armaments_Fire_Base_2,
        Plane_Armaments_Fire_Base_1,
        Plane_Armaments_Fire_Base_0,
        Plane_Armaments_Ammo_Box_Capacity,
        Plane_Armaments_Ammo_Box_Count,
        Plane_Armaments_Total_Ammo,
        Plane_Armaments_Gun_Rotation_Range
    };

    PlaneArmamentsItem(BaseItem* parent = nullptr) : BaseItem(ItemType::Plane_Armaments_Item_Type, parent) {}
    PlaneArmamentsItem(QJsonObject plane_armaments_json, BaseItem* parent = nullptr);
};

/**
 * @brief The PlaneCrewItem class
 */
class PlaneCrewItem : public BaseItem
{
public:
    enum PlaneCrewItemCols {
        Plane_Crew_Name = 0,
        Plane_Crew_Role,
        Plane_Crew_Ability_Unrestricted_Maneuvers,
        Plane_Crew_Ability_Ignores_Deflections,
        Plane_Crew_Can_Drop_Payloads
    };

    enum CrewRoles {
        Pilot = 0,
        CoPilot,
        Observer,
        Gunner
    };

    PlaneCrewItem(BaseItem* parent = nullptr): BaseItem(ItemType::Plane_Crew_Item_Type, parent) {}
    PlaneCrewItem(QJsonObject plane_crew_json, BaseItem* parent = nullptr);
};

/**
 * @brief The GameItem class
 */
class GameItem : public BaseItem
{
public:
    enum GameItemCols {
        Game_Player = 0,        ///< Player Name. Can pull the player's computer username
        Game_Plane_Selected,    ///< Plane QPersistentModelIndex
        Game_Conflict_Name,     ///< Designated name of the conflict
        Game_Conflict_Date      ///< Date at which the conflict supposedly took place
    };

    GameItem(BaseItem* parent = nullptr) : BaseItem(ItemType::Game_Item_Type, parent) {}
};

/**
 * @brief The TurnItem class
 */
class TurnItem : public BaseItem
{
public:
    enum TurnItemCols {
        Turn_Number = 0,
        Turn_Selected_Maneuver,     ///< QPersistentModelIndex of the selected maneuver for the turn
        Turn_Elevation_Delta,       ///< If the plane climbed or dove and by how much
        Turn_Maneuver_Direction,    ///< Left, Straight, Right (LSR)
        Turn_Maneuver_Speed,        ///< Speed of the chosen maneuver
        Turn_Fuel_Consumed,         ///< Amount of fuel consumed this turn
        Turn_Damage_Taken_Engine,   ///< Amount of engine damage taken this turn
        Turn_Damage_Taken_Wing,     ///< Amount of wing damage taken this turn
        Turn_Damage_Taken_Fuse,     ///< Amount of fuselage damage taken this turn
        Turn_Damage_Taken_Tail,     ///< Amount of tail damage taken this turn
        Turn_Active_Effects         ///< List of currently active effects paired with their remaining duration
    };

    enum ActiveEffectIDs {
        Effect_No_Restricted_Maneuvers = 0,
        Effect_No_Speed_3_or_4,
        Effect_Smoke,
        Effect_Fire,
        Effect_Force_Slower_Maneuver,
        Effect_Rudder_Jam_Left,
        Effect_Rudder_Jam_Right,
        Effect_Fuel_Tank_Hit_3,
        Effect_Fuel_Tank_Hit_6
    };

    struct Effect {
        int id;
        int remaining_turns;
        QString desc;

        bool operator==(Effect &e) {
            if ((id == Effect_Rudder_Jam_Left || id == Effect_Rudder_Jam_Right) &&
                (e.id == Effect_Rudder_Jam_Left || e.id == Effect_Rudder_Jam_Right)) {
                return true;
            }
            return id == e.id;
        };
    };

    TurnItem(BaseItem* parent = nullptr) : BaseItem(ItemType::Turn_Item_Type, parent) {}
};

/**
 * @brief The TurnCrewActionItem class
 */
class TurnCrewActionItem : public BaseItem
{
public:
    enum TurnCrewActionItemCols {
        Turn_Crew_Action_Index = 0,     ///< QPersistentModelIndex linking to the crew member that performed this action
        Turn_Crew_Action_Taken,         ///< Enum value of the taken action
        Turn_Crew_Action_Gun_Position,  ///< New position of the assigned armament (will always be 1 for fixed weapons). Map<QPersistentModelIndex, int> is stored since multiple weapons can be assigned
        Turn_Crew_Action_Extra_Data     ///< Extra data regarding a taken action
    };

    enum TurnCrewActionOptions {
        Action_None = 0,                ///< Extra Data: None? (Maybe a random funny string of the observer doing SOMETHING)
        Action_Shoot,                   ///< Extra Data: ShotProperties struct
        Action_Reload,                  ///< Extra Data: None
        Action_Unjam,                   ///< Extra Data: Bool (success/fail)
        Action_Drop_Payload,            ///< Extra Data: Bool (success/fail)
        Action_Observe,                 ///< Extra Data: None
        Action_Custom                   ///< Extra Data: QString of whatever the user input..
    };

    struct ShotProperties {
        int target_delta = 0;           ///< Specifies the altitude delta between this plane and the target
        int burst_len = 0;              ///< The number of bullet units to consume, also modifies damage. Long burst can jam
        bool caused_jam = false;        ///< Whether or not the shot caused a jam. Can only trigger on a long burst
    };

    TurnCrewActionItem(BaseItem* parent = nullptr) : BaseItem(ItemType::Turn_Crew_Action_Item_Type, parent) {}
};

#endif // GAMEMODELITEMS_H
