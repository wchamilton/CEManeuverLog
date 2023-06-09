#ifndef CEMANEUVERS_H
#define CEMANEUVERS_H

#include <QList>
#include <QMap>
#include <QVariant>

Q_DECLARE_METATYPE(QList<int>)
#define IS_RELEASE false

#if IS_RELEASE
static QString GRAPHICS_LOCATION = "./graphics";
#else
static QString GRAPHICS_LOCATION = "../CEManeuverLog/graphics";
#endif

struct Maneuver
{
    enum Directions {
        Stationary = 0,
        North,
        North_East,
        South_East,
        South,
        South_West,
        North_West,
        Left,
        Right
    };
    enum RotationAngle {
        Rot_North = 0,
        Rot_North_East = 60,
        Rot_South_East = 120,
        Rot_South = 180,
        Rot_South_West = 240,
        Rot_North_West = 300
    };
    Maneuver() = default;
    Maneuver(QString name, QString climb_value, QString level_value, QString dive_value, bool causes_spin_check,
             RotationAngle final_rotation, QList<Directions> tile_movements) :
        name(name),
        climb_value(climb_value),
        level_value(level_value),
        dive_value(dive_value),
        causes_spin_check(causes_spin_check),
        final_rotation(final_rotation),
        tile_movements(tile_movements)
    {}
    Maneuver(QString name, RotationAngle final_rotation, QList<Directions> tile_movements,
             bool is_restricted = false, bool is_climb_restricted = false) :
        name(name),
        is_restricted(is_restricted),
        is_climb_restricted(is_climb_restricted),
        final_rotation(final_rotation),
        tile_movements(tile_movements)
    {}

    QString name;
    QString climb_value = "-";
    QString level_value = "-";
    QString dive_value  = "-";
    bool is_restricted = false;
    bool is_climb_restricted = false;
    bool causes_spin_check = false;
    RotationAngle final_rotation = Rot_North;
    QList<Directions> tile_movements;
};

extern const QList<Maneuver> master_maneuver_list;

class BaseItem
{
public:
    enum ItemType {
        Base_Item_Type = 0,
        Plane_Item_Type,
        Maneuver_Item_Type,
        Crew_Item_Type,
        Gun_Link_Item_Type,
        Gun_Item_Type,
        Turn_Item_Type,
        Crew_Turn_Item_Type
    };

    BaseItem(ItemType type, BaseItem* item = nullptr) : type(type), parent(item) {}
    virtual ~BaseItem();

    virtual QVariant data(int column) const;
    virtual void setData(int column, QVariant data);
    void addChild(BaseItem* item) { children << item; }
    BaseItem* childAt(int row) const { return children.size() > row && row >= 0 ? children.at(row) : nullptr; }
    int childRow(BaseItem* item) { return children.indexOf(item); }
    int row() { return parent->childRow(this); }
    BaseItem* getParent() { return parent; }
    int childCount() const { return children.size(); }
    ItemType getType() { return type; }
    void removeChild(int row) { if (row >= 0 && row < children.size()) delete children.takeAt(row); }
    void removeChildren() { qDeleteAll(children); children.clear(); }

private:
    QMap<int, QVariant> column_data;
    ItemType type;
    BaseItem* parent = nullptr;
    QList<BaseItem*> children;
};

#endif // CEMANEUVERS_H
