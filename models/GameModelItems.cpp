#include "GameModelItems.h"
#include <QJsonObject>

PlaneItem::PlaneItem(QJsonObject plane_json, BaseItem* parent) : BaseItem(ItemType::Plane_Item_Type, parent)
{

}

PlaneManeuverItem::PlaneManeuverItem(QJsonObject plane_maneuver_json, BaseItem *parent) : BaseItem(ItemType::Plane_Maneuver_Item_Type, parent)
{

}

PlaneArmamentsItem::PlaneArmamentsItem(QJsonObject plane_armaments_json, BaseItem *parent) : BaseItem(ItemType::Plane_Armaments_Item_Type, parent)
{

}

PlaneCrewItem::PlaneCrewItem(QJsonObject plane_crew_json, BaseItem *parent) : BaseItem(ItemType::Plane_Crew_Item_Type, parent)
{

}
