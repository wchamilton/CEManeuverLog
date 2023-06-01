#include "TurnItems.h"
#include <QPersistentModelIndex>

TurnItem::TurnItem(QPersistentModelIndex maneuver_idx, int alt, BaseItem *parent) : BaseItem(Turn_Item_Type, parent)
{
    setData(Turn_Maneuver_Col, maneuver_idx);
    setData(Turn_Altitude_Col, alt);
}

TurnCrewItem::TurnCrewItem(QPersistentModelIndex crew_idx, QString action, BaseItem *parent) : BaseItem(Crew_Turn_Item_Type, parent)
{
    setData(Turn_Crew_Col, crew_idx);
    setData(Turn_Action_Col, action);
}
