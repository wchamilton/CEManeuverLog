#include "TurnItems.h"
#include <QPersistentModelIndex>

TurnItem::TurnItem(QPersistentModelIndex maneuver_idx, int alt, BaseItem *parent) : BaseItem(Turn_Item_Type, parent)
{
    setData(Turn_Maneuver_Col, maneuver_idx);
    setData(Turn_Altitude_Col, alt);
}

TurnCrewItem::TurnCrewItem(QPersistentModelIndex crew_idx, int action, QVariant action_decorator, BaseItem *parent) : BaseItem(Crew_Turn_Item_Type, parent)
{
    setData(Turn_Crew_Col, crew_idx);
    setData(Turn_Action_Col, action);
    setData(Turn_Action_Decorator_Col, action_decorator);
}
