#ifndef PLANEITEMS_H
#define PLANEITEMS_H

#include <QJsonObject>
#include <QVariant>

#include "CEManeuvers.h"

class PlaneItem : public BaseItem
{
public:
    enum RudderStates {
        Rudder_Normal = 0,
        Rudder_Jammed_Left,
        Rudder_Jammed_Right
    };

    enum PlaneItemCols {
        Plane_Name = 0,
        Plane_Era,
        Fuel,
        Engine_HP,
        Engine_Critical,
        Wing_HP,
        Wing_Critical,
        Fuselage_HP,
        Fuselage_Critical,
        Tail_HP,
        Tail_Critical,
        Rated_Climb,
        Rated_Dive,
        Max_Altitude,
        Can_Return_To_Max_Alt,
        Stability,
        Is_On_Fire,
        Is_Gliding,
        Is_Spinning,
        Rudder_State,
        Rudder_Jam_Duration,
        Bombs_Carried,
        COL_COUNT
    };
    PlaneItem(QJsonObject plane, BaseItem* parent = nullptr);
    PlaneItem(BaseItem* parent = nullptr);
    QJsonObject toJSON() const;
};
Q_DECLARE_METATYPE(PlaneItem::RudderStates)

class ManeuverItem : public BaseItem
{
public:
    enum ManeuverItemCols {
        Maneuver_Name = 0,
        IsEnabled,
        Can_Be_Used,
        Added_To_Schedule,
        Speed,
        Direction,
        Climb_Value,
        Level_Value,
        Dive_Value,
        Can_Be_Repeated,
        Can_Reload,
        Can_Put_Out_Fires,
        Observer_Can_Reload,
        Is_Restricted,
        Is_Weight_Restricted,
        Is_Climb_Restricted,
        Causes_Spin_Check,
        COL_COUNT
    };
    ManeuverItem(QJsonObject maneuver, BaseItem* parent = nullptr);
    ManeuverItem(Maneuver maneuver, BaseItem* parent = nullptr);
    QJsonObject toJSON() const;
};

class CrewItem : public BaseItem
{
public:
    enum CrewCols {
        Crew_Name = 0,
        Crew_Role,
        Has_Unrestricted_Maneuvers,
        Has_Ignore_Deflection,
        Can_Drop_Bombs,
        Wounds,
        COL_COUNT
    };
    CrewItem(QJsonObject crew, BaseItem* parent = nullptr);
    CrewItem(BaseItem* parent = nullptr);
    QJsonObject toJSON() const;
};

class GunLinkItem : public BaseItem
{
public:
    GunLinkItem(BaseItem* parent = nullptr);
    QVariant data(int column) const override;
};

class GunItem : public BaseItem
{
public:
    enum GunCols {
        Gun_Name = 0,
        Gun_Is_Linked,
        Fire_Template,
        Fire_Base_0,
        Fire_Base_1,
        Fire_Base_2,
        Fire_Base_3,
        Shots_Fired,
        Ammo_Box_Capacity,
        Ammo_Box_Count,
        Ammo_In_Current_Box,
        Total_Ammo_Remaining,
        Total_Ammo,
        Gun_Destroyed,
        Gun_Position,
        Gun_Last_Position,
        Gun_Position_Range,
        COL_COUNT
    };
    GunItem(QJsonObject gun, BaseItem* parent = nullptr);
    GunItem(BaseItem* parent = nullptr);
    QVariant data(int column) const;
    QJsonObject toJSON() const;
};

#endif // PLANEITEMS_H
