#ifndef HEXTILE_H
#define HEXTILE_H

#include <QGraphicsItem>

class HexTile : public QGraphicsItem
{
public:
    enum TileType {
        Blank_Tile = 0,
        Starting_Tile,
        Destination_Tile,
        Spin_Tile
    };
    enum RotationAngle {
        North = 0,
        North_East = 60,
        South_East = 120,
        South = 180,
        South_West = 240,
        North_West = 300
    };

    HexTile(TileType type, qreal rotation, QGraphicsItem *parent = nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void setHovered(bool hovered) { this->hovered = hovered; }
    void setWeightRestricted(bool is_weight_restricted) { this->is_weight_restricted = is_weight_restricted; }
    void setIsAvailable(bool is_available) { this->is_available = is_available; }
    void setSelected(bool selected) { is_selected = selected; QGraphicsItem::setSelected(selected); }
    QPolygon getHexPoly() { return hex_poly; }

private:
    TileType tile_type;
    bool hovered = false;
    bool is_weight_restricted = false;
    bool is_available = true;
    bool is_selected = false;
    QPolygon hex_poly;
};

#endif // HEXTILE_H
