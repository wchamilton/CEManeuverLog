#ifndef PLANEITEMS_H
#define PLANEITEMS_H

#include <QJsonObject>
#include <QVariant>

class BaseItem
{
public:
    BaseItem(BaseItem* item = nullptr) : parent(item) {}
    ~BaseItem() { qDeleteAll(children); }

    QVariant data(int column) const;
    void setData(int column, QVariant data);
    void addChild(BaseItem* item) { children << item; }
    BaseItem* childAt(int row) { return children.size() > row ? children.at(row) : nullptr; }
    int childRow(BaseItem* item) { return children.indexOf(item); }
    int row() { return parent->childRow(this); }
    BaseItem* getParent() { return parent; }
    int childCount() { return children.size(); }

private:
    QMap<int, QVariant> column_data;
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
};

class ManeuverItem : public BaseItem
{
public:
    enum ManeuverItemCols {
        Maneuver_Name = 0,
        Speed,
        Direction,
        Climb_Value,
        Level_Value,
        Dive_Value,
        Can_Reload,
        Observer_Can_Reload,
        Is_Restricted,
        Climb_Restricted,
        Can_Put_Out_Fires,
        Is_Bomb_Restricted,
        COL_COUNT
    };
    ManeuverItem(QJsonObject maneuver, BaseItem* parent = nullptr);
};

class CrewItem : public BaseItem
{
public:
    enum CrewCols {
        Crew_Name = 0,
        Crew_Role,
        Reds_Earned,
        Has_Gun,
        Ammo_Box_Size,
        Ammo_Amount,
        Fire_Template,
        Fire_Base_3,
        Fire_Base_2,
        Fire_Base_1,
        Fire_Base_0,
        Gun_Destroyed,
        COL_COUNT
    };
    CrewItem(QJsonObject crew, BaseItem* parent = nullptr);
};

#endif // PLANEITEMS_H
