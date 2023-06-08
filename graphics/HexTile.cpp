#include "HexTile.h"

#include <QPen>
#include <QPainter>
#include <QDebug>
#include <cmath>

HexTile::HexTile(TileType type, qreal rotation, QGraphicsItem *parent) :
    QGraphicsItem(parent),
    tile_type(type)
{
    setTransformOriginPoint(50, (sqrt(3)*25));
    setRotation(rotation);
    if (type == Starting_Tile) {
        setRotation(rotation + 180);
    }

    setupHexPoly();
}

HexTile::HexTile(HexTile::TileType type, QGraphicsItem *parent) :
    QGraphicsItem(parent),
    tile_type(type)
{
    setTransformOriginPoint(QPoint(50, (sqrt(3)*25)));
    setupHexPoly();
}

void HexTile::setupHexPoly()
{
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
    painter->fillPath(path, is_weight_restricted || !is_available ? Qt::lightGray : Qt::white);
    if (is_selected) {
        pen.setColor(Qt::blue);
        pen.setWidth(12);
        painter->setBrush(QColor(Qt::yellow));
    }
    if (hovered) {
        pen.setWidth(15);
    }
    painter->setPen(pen);
    painter->drawPolygon(hex_poly);
    painter->setPen(old_pen);

    switch (tile_type) {
    case Starting_Tile: {
        QFont font = painter->font();
        font.setPixelSize(40);
        font.setBold(true);
        painter->setFont(font);
        painter->drawText(boundingRect(), Qt::AlignCenter, "V");
        break;
    }
    case Spin_Tile: {
        QFont font = painter->font();
        font.setPixelSize(70);
        font.setBold(true);
        painter->setFont(font);
        painter->drawText(boundingRect(), Qt::AlignCenter, "?");
        break;
    }
    case Plane_Icon_Tile: {
//        QPixmap plane_png("./graphics/plane.png");
        QPixmap plane_png("../CEManeuverLog/graphics/plane.png");
        qreal plane_width = 2*boundingRect().width()/3;
        qreal plane_height = 2*boundingRect().height()/3;
        painter->drawPixmap(boundingRect().width()/2 - plane_width/2, boundingRect().height()/2 - 5*plane_height/12,
                            plane_width, plane_height, plane_png, boundingRect().x(), boundingRect().y(), 0, 0);
        break;
    }
    case H_Tile: {
        QFont font = painter->font();
        font.setPixelSize(40);
        painter->setFont(font);
        painter->drawText(boundingRect(), Qt::AlignCenter, "H");
        break;
    }
    case H_S_Tile: {
        QFont font = painter->font();
        font.setPixelSize(40);
        painter->setFont(font);
        painter->drawText(boundingRect(), Qt::AlignCenter, "H/S");
        break;
    }
    case H_L_Tile: {
        QFont font = painter->font();
        font.setPixelSize(40);
        painter->setFont(font);
        painter->drawText(boundingRect(), Qt::AlignCenter, "H/L");
        break;
    }
    case S_Tile: {
        QFont font = painter->font();
        font.setPixelSize(40);
        painter->setFont(font);
        painter->drawText(boundingRect(), Qt::AlignCenter, "S");
        break;
    }
    case S_L_Tile: {
        QFont font = painter->font();
        font.setPixelSize(40);
        painter->setFont(font);
        painter->drawText(boundingRect(), Qt::AlignCenter, "S/L");
        break;
    }
    case L_Tile: {
        QFont font = painter->font();
        font.setPixelSize(40);
        painter->setFont(font);
        painter->drawText(boundingRect(), Qt::AlignCenter, "L");
        break;
    }
    case All_Tile: {
        QFont font = painter->font();
        font.setPixelSize(40);
        painter->setFont(font);
        painter->drawText(boundingRect(), Qt::AlignCenter, "ALL");
        break;
    }
    case Blind_Spot_1: {
        QFont font = painter->font();
        font.setPixelSize(20);
        font.setBold(true);
        painter->setFont(font);
        painter->drawText(boundingRect(), Qt::AlignCenter, "Blind\nSpot\n1");
        break;
    }
    case Blind_Spot_2: {
        QFont font = painter->font();
        font.setPixelSize(20);
        font.setBold(true);
        painter->setFont(font);
        painter->drawText(boundingRect(), Qt::AlignCenter, "Blind\nSpot\n2");
        break;
    }
    default: break;
    }
}
