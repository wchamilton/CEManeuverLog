#ifndef PLANEITEMS_H
#define PLANEITEMS_H

#include <QJsonObject>
#include <QVariant>

class Maneuver;
class BaseItem
{
public:
    enum ItemType {
        Base_Item_Type = 0,
        Plane_Item_Type,
        Maneuver_Item_Type,
        Crew_Item_Type,
        Gun_Item_Type
    };

    BaseItem(ItemType type, BaseItem* item = nullptr) : type(type), parent(item) {}
    virtual ~BaseItem();

    QVariant data(int column) const;
    void setData(int column, QVariant data);
    void addChild(BaseItem* item) { children << item; }
    BaseItem* childAt(int row) const { return children.size() > row ? children.at(row) : nullptr; }
    int childRow(BaseItem* item) { return children.indexOf(item); }
    int row() { return parent->childRow(this); }
    BaseItem* getParent() { return parent; }
    int childCount() const { return children.size(); }
    ItemType getType() { return type; }

private:
    QMap<int, QVariant> column_data;
    ItemType type;
    BaseItem* parent = nullptr;
    QList<BaseItem*> children;
};

class PlaneItem : public BaseItem
{
public:
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
        Bombs_Carried,
        COL_COUNT
    };
    PlaneItem(QJsonObject plane, BaseItem* parent = nullptr);
    PlaneItem(BaseItem* parent = nullptr);
    QJsonObject toJSON() const;
};

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
        Wounds,
        COL_COUNT
    };
    CrewItem(QJsonObject crew, BaseItem* parent = nullptr);
    CrewItem(BaseItem* parent = nullptr);
    QJsonObject toJSON() const;
};

class GunItem : public BaseItem
{
public:
    enum GunCols {
        Gun_Name,
        Gun_Count,
        Fire_Template,
        Fire_Base_3,
        Fire_Base_2,
        Fire_Base_1,
        Fire_Base_0,
        Ammo_Box_Capacity,
        Ammo_Box_Count,
        Gun_Destroyed,
        COL_COUNT
    };
    GunItem(QJsonObject gun, BaseItem* parent = nullptr);
    GunItem(BaseItem* parent = nullptr);
    QJsonObject toJSON() const;
};

#endif // PLANEITEMS_H
