#ifndef HEXTILE_H
#define HEXTILE_H

#include <QGraphicsItem>

class HexTile : public QGraphicsItem
{
public:
    enum TileType {
        Blank_Tile = 0,
        Starting_Tile,
        Plane_Icon_Tile,
        Spin_Tile,
        H_Tile,
        H_S_Tile,
        H_L_Tile,
        S_Tile,
        S_L_Tile,
        L_Tile,
        All_Tile,
        Blind_Spot_1,
        Blind_Spot_2
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
    HexTile(TileType type, QGraphicsItem *parent = nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void setHovered(bool hovered) { this->hovered = hovered; }
    void setWeightRestricted(bool is_weight_restricted) { this->is_weight_restricted = is_weight_restricted; }
    void setIsAvailable(bool is_available) { this->is_available = is_available; }
    void setSelected(bool selected) { is_selected = selected; }
    QPolygon getHexPoly() { return hex_poly; }

private:
    void setupHexPoly();

    TileType tile_type;
    bool hovered = false;
    bool is_weight_restricted = false;
    bool is_available = true;
    bool is_selected = false;
    QPolygon hex_poly;
};

#endif // HEXTILE_H
