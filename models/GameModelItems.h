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
        Maneuver_Final_Rotation
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
        Plane_Fuel_Cap,
        Plane_Max_Climb,
        Plane_Max_Dive,
        Plane_Stability_Rating,
        Plane_Crew_Roles,
    };

    PlaneItem(QJsonObject plane_json, BaseItem* parent = nullptr);
};

/**
 * @brief The PlaneManeuverItem class
 */
class PlaneManeuverItem : public BaseItem
{
public:
    enum PlaneManeuverItemCols {
        Plane_Maneuver_Tolerances = 0,
        Plane_Maneuver_Climb_Val,
        Plane_Maneuver_Level_Val,
        Plane_Maneuver_Dive_Val,
        Plane_Maneuver_Is_Weight_Restricted,
    };

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
        Plane_Armaments_Gun_Is_Linked,
        Plane_Armaments_Fire_Template,
        Plane_Armaments_Fire_Base_0,
        Plane_Armaments_Fire_Base_1,
        Plane_Armaments_Fire_Base_2,
        Plane_Armaments_Fire_Base_3,
        Plane_Armaments_Ammo_Box_Capacity,
        Plane_Armaments_Ammo_Box_Count,
        Plane_Armaments_Total_Ammo,
        Plane_Armaments_Gun_Position_Range,
    };

    PlaneArmamentsItem(QJsonObject plane_armaments_json, BaseItem* parent = nullptr);
};

class PlaneCrewItem : BaseItem
{
public:
    enum PlaneCrewItemCols {
        Plane_Crew_Name = 0,
        Plane_Crew_Role,
        Plane_Crew_Ability_Unrestricted_Maneuvers,
        Plane_Crew_Ability_Ignores_Deflections,
        Plane_Crew_Assigned_Armaments
    };

    PlaneCrewItem(QJsonObject plane_crew_json, BaseItem* parent = nullptr);
};

/**
 * @brief The ChitItem class
 */
class ChitItem : public BaseItem
{
public:
    enum ChitItemCols {
        Chit_Description = 0,       ///< Description of what the chit does
        Chit_Effect_Duration,       ///< Number of turns the effect lasts. -1 is perpetual. 0 is an immediate effect.
        Chit_Superceded_By          ///< QPersistentModelIndexList of chits that would have priority over this
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

    GameItem(BaseItem* parent = nullptr);
    GameItem(QJsonObject game_json, BaseItem* parent = nullptr);
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
        Turn_Chit_Effects_Applied,  ///< QPersistentModelIndexList of the chit effects applied during the turn
    };

    TurnItem(BaseItem* parent = nullptr) : BaseItem(ItemType::Turn_Item_Type) {}
};

/**
 * @brief The TurnCrewActionItem class
 */
class TurnCrewActionItem : public BaseItem
{
public:
    struct ShotProperties {
        int target_delta = 0;           ///< Specifies the altitude delta between this plane and the target
        int burst_len = 0;              ///< The number of bullet units to consume, also modifies damage. Long burst can jam
        bool caused_jam = false;        ///< Whether or not the shot caused a jam. Can only trigger on a long burst
    };

    enum TurnCrewActionOptions {
        Action_Shoot = 0,               ///< Extra Data: ShotProperties struct
        Action_Reload,                  ///< Extra Data: None
        Action_Unjam,                   ///< Extra Data: Bool (success/fail)
        Action_Drop_Payload,            ///< Extra Data: Bool (success/fail)
        Action_Observe,                 ///< Extra Data: None
        Action_Custom                   ///< Extra Data: QString of whatever the user input..
    };

    enum TurnCrewActionItemCols {
        Turn_Crew_Action_Index = 0,     ///< QPersistentModelIndex linking to the crew member that performed this action
        Turn_Crew_Action_Taken,         ///< Enum value of the taken action
        Turn_Crew_Action_Extra_Data     ///< Extra data regarding a taken action
    };

    TurnCrewActionItem(BaseItem* parent = nullptr) : BaseItem(ItemType::Turn_Crew_Action_Item_Type) {}
};

#endif // GAMEMODELITEMS_H
