#ifndef CEMANEUVERS_H
#define CEMANEUVERS_H

#include <QList>
#include <QMap>
#include <QVariant>
#include <QPointF>

Q_DECLARE_METATYPE(QList<int>)
#define IS_RELEASE false

#if IS_RELEASE
static QString GRAPHICS_LOCATION = "./graphics";
static QString PLANES_LOCATION = "./Planes";
static QString CHITS_LOCATION = "./chits";
#else
static QString GRAPHICS_LOCATION = "../../../CEManeuverLog/graphics";
static QString PLANES_LOCATION = "../../../CEManeuverLog/Planes";
static QString CHITS_LOCATION = "../../../CEManeuverLog/chits";
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
    Maneuver(QString name, QString tolerances, bool causes_spin_check,
             RotationAngle final_rotation, QList<Directions> tile_movements, QPointF pos) :
        name(name),
        tolerances(tolerances),
        causes_spin_check(causes_spin_check),
        final_rotation(final_rotation),
        tile_movements(tile_movements),
        pos(pos)
    {}
    Maneuver(QString name, RotationAngle final_rotation, QList<Directions> tile_movements, QPointF pos,
             bool is_restricted = false, bool is_climb_restricted = false) :
        name(name),
        is_restricted(is_restricted),
        has_climb_condition(is_climb_restricted),
        final_rotation(final_rotation),
        tile_movements(tile_movements),
        pos(pos)
    {}

    QString name;
    QString tolerances = "-/-/-";
    bool is_restricted = false;
    bool has_climb_condition = false;
    bool causes_spin_check = false;
    RotationAngle final_rotation = Rot_North;
    QList<Directions> tile_movements;
    QPointF pos;
};

extern const QMap<QString, Maneuver> master_maneuver_map;

class BaseItem
{
public:
    enum ItemType {
        Base_Item_Type = 0,
        Maneuver_Item_Type,
        Plane_Item_Type,
        Plane_Maneuver_Item_Type,
        Plane_Armaments_Item_Type,
        Plane_Crew_Item_Type,
        Active_Effect_Item_Type,
        Chit_Item_Type,
        Game_Item_Type,
        Turn_Item_Type,
        Turn_Crew_Item_Type,
        Turn_Armament_Item_Type
    };

    BaseItem(ItemType type, BaseItem* parent = nullptr) : type(type), parent(parent) {}
    virtual ~BaseItem();

    virtual QVariant data(int column) const;
    virtual void setData(int column, const QVariant &data);

    ItemType getType() { return type; }
    BaseItem* childAt(int row) const { return children.size() > row && row >= 0 ? children.at(row) : nullptr; }
    BaseItem* getParent() { return parent; }
    int childCount() const { return children.size(); }
    int childRow(const BaseItem* item) const { return children.indexOf(item); }
    int columnCount() const { return column_data.count(); }
    int row() const { return parent->childRow(this); }
    void addChild(BaseItem* item) { children << item; }
    void removeChild(int row) { if (row >= 0 && row < children.size()) delete children.takeAt(row); }
    void removeChildren() { qDeleteAll(children); children.clear(); }

private:
    QMap<int, QVariant> column_data;
    ItemType type;
    BaseItem* parent = nullptr;
    QList<BaseItem*> children;
};

#endif // CEMANEUVERS_H
