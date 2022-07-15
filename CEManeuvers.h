#ifndef CEMANEUVERS_H
#define CEMANEUVERS_H

#include <QList>
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

#endif // CEMANEUVERS_H
