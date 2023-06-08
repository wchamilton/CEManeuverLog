#ifndef TURNITEM_H
#define TURNITEM_H

#include "CEManeuvers.h"

class TurnItem : public BaseItem
{
public:
    enum TurnItemCols {
        Turn_Number = 0,
        Turn_Maneuver_Col,      // Qt::UserRole will return the maneuver index for the turn
        Turn_Tolerance_Tag,         // C1, C, L, X, D, D1
        Turn_Altitude_Col,
        Turn_Fuel_Consumed,
        Turn_Fuel_Remaining_Col,
        Turn_Item_Col_Count
    };
    TurnItem(QPersistentModelIndex maneuver_idx, int alt, BaseItem* parent = nullptr);
};

class TurnCrewItem : public BaseItem
{
public:
    enum TurnCrewItemCols {
        Turn_Crew_Col = 1,          // Qt::UserRole will return the crew idx
        Turn_Action_Col,
        Turn_Action_Decorator_Col,
        Turn_Crew_Item_Col_Count
    };

    enum ActionTaken {
        No_Action = 0,
        Shot_Action,
        Reload_Action,
        Unjam_Action,
        Observe_Action,
        Drop_Bomb_Action,
        Custom_Action
    };
    enum ShotBitField {
        No_Shot         = 1 << 0,
        Short_Burst     = 1 << 1,
        Medium_Burst    = 1 << 2,
        Long_Burst      = 1 << 3,
        Target_Level    = 1 << 4,
        Target_Above    = 1 << 5,
        Target_Below    = 1 << 6,
        Range_0         = 1 << 7,
        Range_1         = 1 << 8,
        Range_2         = 1 << 9,
        Range_3         = 1 << 10
    };

    TurnCrewItem(QPersistentModelIndex crew_idx, int action, QVariant action_decorator, BaseItem* parent = nullptr);
};

#endif // TURNITEM_H
