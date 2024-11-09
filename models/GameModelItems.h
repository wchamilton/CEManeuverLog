#ifndef GAMEMODELITEMS_H
#define GAMEMODELITEMS_H

#include "CEManeuvers.h"

/**
 * @brief The ManeuversItem class
 */
class ManeuversItem : public BaseItem
{
public:
    enum ManeuversItemCols {
        Maneuver_Name = 0,
        Maneuver_Force_Spin_Check,
        Maneuver_Tile_Movements,
        Maneuver_Final_Rotation,
        Maneuver_Speed,
        Maneuver_Direction,
        Maneuver_Can_Be_Repeated,
        Maneuver_Can_Reload,
        Maneuver_Observer_Can_Reload,
        Maneuver_Put_Out_Fires_Bonus,
        Maneuver_Is_Climb_Restricted
    };

    ManeuversItem(BaseItem* parent = nullptr) : BaseItem(ItemType::Maneuver_Item_Type, parent) {}
};

/**
 * @brief The PlaneItem class
 */
class PlaneItem : public BaseItem
{
public:
    enum PlaneItemCols {
        Plane_Model = 0,
        Plane_History,
        Plane_Era,
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
        Plane_Stability_Rating,
        Plane_Crew_Roles
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
        Plane_Maneuver_Index = 0,
        Plane_Maneuver_Tolerances,
        Plane_Maneuver_Climb_Val,
        Plane_Maneuver_Level_Val,
        Plane_Maneuver_Dive_Val,
        Plane_Maneuver_Is_Weight_Restricted
    };

    PlaneManeuverItem(QJsonObject plane_maneuver_json, QPersistentModelIndex maneuver_index, BaseItem* parent = nullptr);
};

/**
 * @brief The PlaneArmamentsItem class
 */
class PlaneArmamentsItem : public BaseItem
{
public:
    enum PlaneArmamentsItemCols {
        Plane_Armaments_Name = 0,
        Plane_Armaments_Gun_Is_Linked,
        Plane_Armaments_Fire_Template,
        Plane_Armaments_Fire_Base_3,
        Plane_Armaments_Fire_Base_2,
        Plane_Armaments_Fire_Base_1,
        Plane_Armaments_Fire_Base_0,
        Plane_Armaments_Ammo_Box_Capacity,
        Plane_Armaments_Ammo_Box_Count,
        Plane_Armaments_Total_Ammo,
        Plane_Armaments_Gun_Position_Range
    };

    PlaneArmamentsItem(QJsonObject plane_armaments_json, BaseItem* parent = nullptr);
};

/**
 * @brief The PlaneCrewItem class
 */
class PlaneCrewItem : BaseItem
{
public:
    enum PlaneCrewItemCols {
        Plane_Crew_Name = 0,
        Plane_Crew_Role,
        Plane_Crew_Ability_Unrestricted_Maneuvers,
        Plane_Crew_Ability_Ignores_Deflections,
        Plane_Crew_Assigned_Armaments,
        Plane_Crew_Can_Drop_Payloads
    };

    enum CrewRoles {
        Pilot = 0,
        CoPilot,
        Observer
    };

    PlaneCrewItem(BaseItem* parent = nullptr): BaseItem(ItemType::Plane_Crew_Item_Type, parent) {}
};

/**
 * @brief The ChitItem class
 */
class ChitItem : public BaseItem
{
public:
    enum ChitItemCols {
        Chit_ID = 0,            ///< Enum ID of the chit
        Chit_Colour,            ///< Blue or red. Red is generally more impactful
        Chit_Description,       ///< Description of what the chit does
        Chit_Extra_Effects,     ///< Certain chits will trigger extra effects such as spin checks or crew wound checks
        Chit_Effect_Duration,   ///< Number of turns the effect lasts. -1 is perpetual. 0 is an immediate effect.
        Chit_Image_Path,        ///< QString path to the image
        Chit_Superceded_By      ///< QPersistentModelIndexList of chits that would have priority over this
    };

    enum ChitItemExtraEffects {
        Chit_Extra_Effect_Spin_Check = 0,
        Chit_Extra_Effect_Crew_Wound_Check,
    };

    ChitItem(BaseItem* parent = nullptr) : BaseItem(ItemType::Chit_Item_Type, parent) {}
};

/**
 * @brief The GameItem class
 */
class GameItem : public BaseItem
{
public:
    enum GameItemCols {
        Game_Player = 0,
        Game_Plane_Selected,
        Game_Conflict_Date
    };

    GameItem(BaseItem* parent = nullptr) : BaseItem(ItemType::Game_Item_Type) {}
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
        Turn_Fuel_Consumed,         ///< Amount of fuel consumed this turn
        Turn_Chit_Effects_Applied   ///< QPersistentModelIndexList of the chit effects applied during the turn
    };

    TurnItem(BaseItem* parent = nullptr) : BaseItem(ItemType::Turn_Item_Type) {}
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
        Turn_Crew_Action_Extra_Data     ///< Extra data regarding a taken action
    };

    enum TurnCrewActionOptions {
        Action_Shoot = 0,               ///< Extra Data: ShotProperties struct
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

    TurnCrewActionItem(BaseItem* parent = nullptr) : BaseItem(ItemType::Turn_Crew_Action_Item_Type) {}
};

#endif // GAMEMODELITEMS_H
