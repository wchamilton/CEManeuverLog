#include "HexTile.h"

#include <QPen>
#include <QPainter>
#include <QDebug>
#include <cmath>

HexTile::HexTile(TileType type, qreal rotation, QGraphicsItem *parent) :
    QGraphicsItem(parent),
    tile_type(type)
{
    setTransformOriginPoint(QPoint(50, (sqrt(3)*25)));
    setRotation(rotation);
    if (type == Starting_Tile) {
        setRotation(rotation + 180);
    }

    // Prep the hexagon
    qreal top_bottom_width = boundingRect().width()/2;
    qreal dist_from_edge = top_bottom_width/2;
    hex_poly << QPoint(dist_from_edge, 0) <<
            QPoint(dist_from_edge + top_bottom_width, 0) <<
            QPoint(boundingRect().width(), boundingRect().height()/2) <<
            QPoint(dist_from_edge + top_bottom_width, boundingRect().height()) <<
            QPoint(dist_from_edge, boundingRect().height()) <<
            QPoint(0, boundingRect().height()/2) <<
            QPoint(dist_from_edge, 0);
}

QRectF HexTile::boundingRect() const
{
    qreal size = 50;
    return QRectF(0, 0, 2 * size, sqrt(3)*size);
}

void HexTile::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen old_pen = painter->pen();

    QPen pen;
    pen.setWidth(3);
    QPainterPath path;
    path.addPolygon(hex_poly);
    painter->fillPath(path, is_weight_restricted ? Qt::lightGray : Qt::white);
    if (is_selected) {
        pen.setColor(Qt::darkGreen);
        pen.setWidth(8);
    }
    if (hovered) {
        pen.setWidth(10);
    }
    painter->setPen(pen);
    painter->drawPolygon(hex_poly);
    painter->setPen(old_pen);

    // Carot
    if (tile_type == Starting_Tile) {
        QFont font = painter->font();
        font.setPixelSize(40);
        font.setBold(true);
        painter->setFont(font);
        painter->drawText(QPoint(boundingRect().width()/2 - painter->fontMetrics().width("V")/2 - 1, // -1 to account for the pen width
                                  boundingRect().height()/2 + painter->fontMetrics().height()/2 - 3), "V");
    }
    else if (tile_type == Spin_Tile) {
        QFont font = painter->font();
        font.setPixelSize(70);
        font.setBold(true);
        painter->setFont(font);
        painter->drawText(QPoint(boundingRect().width()/2 - painter->fontMetrics().width("?")/2,
                                 boundingRect().height()/2 + painter->fontMetrics().height()/2 - 20), "?");
    }
    else if (tile_type == Destination_Tile) {
        QPixmap plane_png("../CEManeuverLog/graphics/plane.png");
        qreal plane_width = 2*boundingRect().width()/3;
        qreal plane_height = 2*boundingRect().height()/3;
        painter->drawPixmap(boundingRect().width()/2 - plane_width/2, boundingRect().height()/2 - 5*plane_height/12,
                            plane_width, plane_height, plane_png, boundingRect().x(), boundingRect().y(), 0, 0);
    }
}
