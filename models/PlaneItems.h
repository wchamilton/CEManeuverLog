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
    ~BaseItem() { qDeleteAll(children); }

    QVariant data(int column) const;
    void setData(int column, QVariant data);
    void addChild(BaseItem* item) { children << item; }
    BaseItem* childAt(int row) { return children.size() > row ? children.at(row) : nullptr; }
    int childRow(BaseItem* item) { return children.indexOf(item); }
    int row() { return parent->childRow(this); }
    BaseItem* getParent() { return parent; }
    int childCount() { return children.size(); }
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
        Engine_HP,
        Engine_Critical,
        Wing_HP,
        Wing_Critical,
        Fuselage_HP,
        Fuselage_Critical,
        Bombs_Carried,
        Tail_HP,
        Tail_Critical,
        Fuel,
        Dive,
        Climb,
        Altitude,
        Can_Return_To_Max_Alt,
        Stability,
        Is_On_Fire,
        COL_COUNT
    };
    PlaneItem(QJsonObject plane, BaseItem* parent = nullptr);
    PlaneItem(BaseItem* parent = nullptr);
};

class ManeuverItem : public BaseItem
{
public:
    enum ManeuverItemCols {
        Maneuver_Name = 0,
        IsEnabled,
        Can_Be_Used,
        Speed,
        Direction,
        Climb_Value,
        Level_Value,
        Dive_Value,
        Can_Reload,
        Observer_Can_Reload,
        Can_Be_Repeated,
        Is_Restricted,
        Climb_Restricted,
        Can_Put_Out_Fires,
        Is_Weight_Restricted,
        Causes_Spin_Check,
        COL_COUNT
    };
    ManeuverItem(QJsonObject maneuver, BaseItem* parent = nullptr);
    ManeuverItem(Maneuver maneuver, BaseItem* parent = nullptr);
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
};

class GunItem : public BaseItem
{
public:
    enum GunCols {
        Gun_Name,
        Fire_Template,
        Fire_Base_3,
        Fire_Base_2,
        Fire_Base_1,
        Fire_Base_0,
        Ammo_Box_Size,
        Ammo_Amount,
        Reds_Earned, // Move to CrewCols
        Gun_Destroyed,
        COL_COUNT
    };
    GunItem(QJsonObject gun, BaseItem* parent = nullptr);
};

#endif // PLANEITEMS_H
