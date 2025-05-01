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
        Plane_Name = 0,                 ///< Name of the plane
        Plane_Era,                      ///< Point during the war that this plane (or version of it) was used in
        Plane_Points,                   ///< Point value indicating the relative "power" of the plane
        Plane_Current_Fuel,             ///< Amount of fuel the plane currently has
        Plane_Fuel_Cap,                 ///< Amount of fuel the plane started with
        Plane_Engine_HP,                ///< Total HP for the engine
        Plane_Engine_Critical,          ///< HP threshold before the engine is considered in critical range
        Plane_Wing_HP,                  ///< Total HP for the wing
        Plane_Wing_Critical,            ///< HP threshold before the wing is considered in critical range
        Plane_Fuselage_HP,              ///< Total HP for the fuse
        Plane_Fuselage_Critical,        ///< HP threshold before the fuse is considered in critical range
        Plane_Tail_HP,                  ///< Total HP for the tail
        Plane_Tail_Critical,            ///< HP threshold before the tail is considered in critical range
        Plane_Rated_Climb,              ///< Amount the plane can climb in one turn
        Plane_Rated_Dive,               ///< Amount the plane can dive in one turn before risking a tear
        Plane_Max_Altitude,             ///< Maximum altitude the plane could reach. Normally planes cannot return to it
        Plane_Can_Return_To_Max_Alt,    ///< true/false if plane can return to the max altitude (indicated by a +)
        Plane_Current_Alt,              ///< Current altitude value for the plane
        Plane_Current_Speed,            ///< Current speed value the plane has been going
        Plane_Stability_Rating,         ///< Character value A, B, or C. C rated planes can alternate L+R without needing S
        Plane_Payload_Count,            ///< Amount of bombs/payloads the plane can drop
        Plane_Active_Effects,           ///< List<Effect> of currently active effects
        PLANE_COL_COUNT
    };

    enum PlaneEra {
        Era_UNKNOWN = -1,
        Era_Early_War,
        Era_Late_War
    };

    enum ActiveEffectIDs {
        Effect_No_Restricted_Maneuvers = 0,
        Effect_No_Speed_3_or_4,
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

    PlaneItem(BaseItem* parent = nullptr) : BaseItem(ItemType::Plane_Item_Type, parent) {}
    PlaneItem(QJsonObject plane_json, BaseItem* parent = nullptr);

    QJsonObject toJSON() override;
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
        Plane_Maneuver_Final_Rotation,
        PLANE_MANEUVER_COL_COUNT
    };

    PlaneManeuverItem(Maneuver maneuver, BaseItem* parent = nullptr);
    PlaneManeuverItem(QJsonObject plane_maneuver_json, BaseItem* parent = nullptr);

    QJsonObject toJSON() override;
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
        Plane_Armaments_Gun_Jammed,
        Plane_Armaments_Gun_Is_Linked,
        Plane_Armaments_Fire_Template,
        Plane_Armaments_Fire_Base_3,
        Plane_Armaments_Fire_Base_2,
        Plane_Armaments_Fire_Base_1,
        Plane_Armaments_Fire_Base_0,
        Plane_Armaments_Shots_Fired,
        Plane_Armaments_Ammo_In_Current_Box,
        Plane_Armaments_Ammo_Box_Capacity,
        Plane_Armaments_Ammo_Box_Count,
        Plane_Armaments_Total_Ammo_Remaining,
        Plane_Armaments_Total_Ammo,
        Plane_Armaments_Gun_Rotation_Current_Pos,
        Plane_Armaments_Gun_Rotation_Last_Turn,
        Plane_Armaments_Gun_Rotation_Range,
        PLANE_ARMAMENTS_COL_COUNT
    };

    PlaneArmamentsItem(BaseItem* parent = nullptr) : BaseItem(ItemType::Plane_Armaments_Item_Type, parent) {}
    PlaneArmamentsItem(QJsonObject plane_armaments_json, BaseItem* parent = nullptr);

    QVariant data(int column) const override;
    QJsonObject toJSON() override;
};

class PlaneArmamentLinkItem : public BaseItem
{
public:
    PlaneArmamentLinkItem(BaseItem* parent = nullptr) : BaseItem(BaseItem::Plane_Armaments_Link_Item_Type, parent) {}
    QVariant data(int column) const override;
};

/**
 * @brief The PlaneCrewItem class
 */
class PlaneCrewItem : public BaseItem
{
public:
    enum PlaneCrewItemCols {
        Plane_Crew_Name = 0,
        Plane_Crew_Role_ID,
        Plane_Crew_Role,
        Plane_Crew_Ability_Unrestricted_Maneuvers,
        Plane_Crew_Ability_Ignores_Deflections,
        Plane_Crew_Can_Drop_Payloads,
        Plane_Crew_Wounds,
        Plane_Crew_Reds,
        Plane_Crew_Kills,
        PLANE_CREW_COLS
    };

    enum CrewRoles {
        Pilot = 0,
        CoPilot,
        Observer,
        Gunner
    };

    PlaneCrewItem(BaseItem* parent = nullptr): BaseItem(ItemType::Plane_Crew_Item_Type, parent) {}
    PlaneCrewItem(QJsonObject plane_crew_json, BaseItem* parent = nullptr);

    QJsonObject toJSON() override;
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
        Game_Conflict_Date,     ///< Date at which the conflict supposedly took place
        GAME_ITEM_COLS
    };

    GameItem(BaseItem* parent = nullptr) : BaseItem(ItemType::Game_Item_Type, parent) {}
    QJsonObject toJSON() override;
};

/**
 * @brief The TurnItem class
 */
class TurnItem : public BaseItem
{
public:
    enum TurnItemCols {
        Turn_Number = 0,
        Turn_Selected_Maneuver,             ///< Name of the selected maneuver
        Turn_Maneuver_Direction,            ///< Left, Straight, Right (LSR)
        Turn_Maneuver_Speed,                ///< Speed of the chosen maneuver
        Turn_Plane_Speed_Last_Turn,         ///< Speed the plane went last turn (or started at for turn 1)
        Turn_Plane_Alt_Last_Turn,           ///< Altitude of the plane last turn (or started at for turn 1)
        Turn_Plane_State_Fuel,              ///< Remaining fuel for the plane
        Turn_Plane_State_Engine_HP,         ///< Current HP value for the engine
        Turn_Plane_State_Wing_HP,           ///< Current HP value for the wings
        Turn_Plane_State_Fuse_HP,           ///< Current HP value for the fuse
        Turn_Plane_State_Tail_HP,           ///< Current HP value for the tail
        Turn_Plane_State_Active_Effects,    ///< List of effects actively applied to the plane
        TURN_PLANE_COL_COUNT
    };

    TurnItem(BaseItem* parent = nullptr) : BaseItem(ItemType::Turn_Item_Type, parent) {}
    QJsonObject toJSON() override;
};

/**
 * @brief The TurnCrewItem class
 */
class TurnCrewItem : public BaseItem
{
public:
    enum TurnCrewItemCols {
        Turn_Crew_Index = 0,            ///< QPersistentModelIndex linking to the crew member that performed this action
        Turn_Crew_Action_Taken,         ///< Enum value of the taken action
        Turn_Crew_Action_Extra_Data,    ///< Extra data regarding a taken action
        Turn_Crew_Wounds_Accrued,       ///< Current amount of wounds received
        Turn_Crew_Total_Red_Hits,       ///< Current amount of red hits inflicted
        Turn_Crew_Total_Kills,          ///< Current amount of kills awarded
        TURN_CREW_COL_COUNT
    };

    enum TurnCrewActionOptions {
        Action_None = 0,                ///< Extra Data: None? (Maybe a random funny string of the observer doing SOMETHING)
        Action_Shoot,                   ///< Extra Data: ShotProperties struct
        Action_Reload,                  ///< Extra Data: QString saying which gun was reloaded
        Action_Unjam,                   ///< Extra Data: Bool (success/fail)
        Action_Drop_Payload,            ///< Extra Data: Bool (success/fail)
        Action_Observe,                 ///< Extra Data: QString saying that an observation was made
        Action_Custom                   ///< Extra Data: QString of whatever the user input..
    };

    struct ShotProperties {
        int target_delta = 0;           ///< Specifies the altitude delta between this plane and the target
        int target_range = 0;           ///< Range at which the shot was taken. Important for range 3
        int burst_len = 0;              ///< Number of bullet units consumed, also modifies damage. Long burst can jam
        bool caused_jam = false;        ///< Whether or not the shot caused a jam. Can only trigger on a long burst
    };

    TurnCrewItem(BaseItem* parent = nullptr) : BaseItem(ItemType::Turn_Crew_Item_Type, parent) {}
    QJsonObject toJSON() override;
};

/**
 * @brief The TurnArmamentItem class
 */
class TurnArmamentItem : public BaseItem
{
public:
    enum TurnArmamentItemCols {
        Turn_Crew_Armament_Index = 0,               ///< QPersistentModelIndex linking to the armament item
        Turn_Crew_Armament_Position,                ///< Current position of the armament (will always be 1 for fixed weapons)
        Turn_Crew_Armament_Is_Destroyed,            ///< Is the gun destroyed (true/false
        Turn_Crew_Armament_IsJammed,                ///< Is the gun jammed (true/false)
        Turn_Crew_Armament_Current_Box_Ammo,        ///< Amount of ammo remaining in the current box
        Turn_Crew_Armament_Remaining_Ammo_Boxes,    ///< Number of remaining ammo boxes
        TURN_CREW_ARMAMENT_COL_COUNT
    };

    TurnArmamentItem(BaseItem* parent = nullptr) : BaseItem(ItemType::Turn_Armament_Item_Type, parent) {}
    QJsonObject toJSON() override;
};

#endif // GAMEMODELITEMS_H
