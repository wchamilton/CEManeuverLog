#ifndef FIRINGARC_H
#define FIRINGARC_H

#include <QGraphicsItem>

class HexTile;
class FiringArc : public QGraphicsItem
{

public:
    FiringArc(QString id, QGraphicsItem* parent = nullptr);
    void addHex(QList<int> gun_positions, HexTile* hex, QList<int> hex_position);
    void setGunPos(int pos);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {}
    QPainterPath shape() const override;

private:
    void moveTile(HexTile* tile, int direction);

    QString fire_template_ID;
    QMap<int,QList<HexTile*>> hexes_by_pos;
};

#endif // FIRINGARC_H
