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
        Turn_Crew_Item_Col_Count
    };

    // Need to make another data container to store more information regarding the target (such as relative alt)
    enum ActionTaken {
        No_Action = 0,
        Short_Burst_Action,
        Medium_Burst_Action,
        Long_Burst_Action,
        Reload_Action,
        Unjam_Action,
        Observe_Action,
        Drop_Bomb_Action
    };

    TurnCrewItem(QPersistentModelIndex crew_idx, int action, BaseItem* parent = nullptr);
};

#endif // TURNITEM_H
