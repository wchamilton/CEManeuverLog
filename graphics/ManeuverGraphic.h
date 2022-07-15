#ifndef MANEUVERGRAPHIC_H
#define MANEUVERGRAPHIC_H

#include <QGraphicsItem>
#include <QPersistentModelIndex>
#include "HexTile.h"
#include "CEManeuvers.h"

class ManeuverGraphic : public QGraphicsItem
{
public:
    enum ShiftText {
        Shift_None = 0,
        Shift_Left,
        Shift_Right
    };

    ManeuverGraphic(ShiftText shift_val = Shift_None, QGraphicsItem *parent = nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void addHex(Maneuver::Directions position, HexTile::TileType tile_type = HexTile::Blank_Tile, qreal rotation = 0);
    QPainterPath shape() const override;
    void setModelIndex(QPersistentModelIndex idx);
    void updateManeuverState();

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

private:
    void moveTile(HexTile* tile, Maneuver::Directions position);
    QString getID() const;
    QString getTolerances() const;
    QString getAdditionalInfo() const;

    QPersistentModelIndex maneuver_idx;
    ShiftText shift_val = Shift_None;
    QRectF textBox;
    QList<Maneuver::Directions> directions_list;
};

#endif // MANEUVERGRAPHIC_H
