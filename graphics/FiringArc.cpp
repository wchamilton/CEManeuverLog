#include "FiringArc.h"
#include "HexTile.h"
#include "CEManeuvers.h"

#include <QPainter>

FiringArc::FiringArc(QString id, QGraphicsItem *parent) :
    QGraphicsItem(parent),
    fire_template_ID(id)
{
    setScale(0.42);
    setAcceptHoverEvents(true);
    setFlag(ItemIsFocusable, true);

    // Every firing arc has the plane hex in there as a reference point
    new HexTile(HexTile::Plane_Icon_Tile, HexTile::North, this);
}

void FiringArc::addHex(QList<int> gun_positions, HexTile *hex, QList<int> hex_position)
{
    hex->setParentItem(this);
    for (int pos : gun_positions) {
        hexes_by_pos[pos] << hex;
    }
    for (int pos : hex_position) {
        moveTile(hex, pos);
    }
}

void FiringArc::setGunPos(int pos)
{
    if (!hexes_by_pos.keys().contains(pos)) {
        if (hexes_by_pos.contains(1)) {
            pos = 1;
        }
        else if (hexes_by_pos.contains(4)) {
            pos = 4;
        }
        else if (!hexes_by_pos.isEmpty()) {
            pos = hexes_by_pos.keys().first();
        }
        else {
            return;
        }
    }
    for (auto hexes : hexes_by_pos) {
        for (auto hex : hexes) {
            hex->setIsAvailable(false);
        }
    }
    for (auto hex : hexes_by_pos[pos]) {
        hex->setIsAvailable(true);
    }
}

QRectF FiringArc::boundingRect() const
{
    // Use the polygon constructed in the shape() function
    return shape().boundingRect();
}

QPainterPath FiringArc::shape() const
{
    QPainterPath path;

    // Need to translate each poly the same way the child hexes were translated
    for (auto child : childItems()) {
        QPolygon poly = static_cast<HexTile*>(child)->getHexPoly();
        poly.translate(child->pos().x(), child->pos().y());
        path.addPolygon(poly);
    }
    return path;
}

void FiringArc::moveTile(HexTile *tile, int direction)
{
    switch (direction) {
        case Maneuver::Stationary: break;
        case Maneuver::North:      tile->moveBy(0, -tile->boundingRect().height()); break;
        case Maneuver::North_East: tile->moveBy(3*tile->boundingRect().width()/4, -tile->boundingRect().height()/2); break;
        case Maneuver::South_East: tile->moveBy(3*tile->boundingRect().width()/4, tile->boundingRect().height()/2); break;
        case Maneuver::South:      tile->moveBy(0, tile->boundingRect().height()); break;
        case Maneuver::South_West: tile->moveBy(-3*tile->boundingRect().width()/4, tile->boundingRect().height()/2); break;
        case Maneuver::North_West: tile->moveBy(-3*tile->boundingRect().width()/4, -tile->boundingRect().height()/2); break;
        case Maneuver::Left:       tile->moveBy(-tile->boundingRect().width(), 0); break;
        case Maneuver::Right:      tile->moveBy(tile->boundingRect().width(), 0); break;
    }
}
