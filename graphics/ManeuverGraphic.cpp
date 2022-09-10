#include "ManeuverGraphic.h"

#include <QPainter>
#include <QPen>
#include <QCursor>
#include <QFontMetrics>
#include <QDebug>

#include "models/PlaneItems.h"

static const QString black_square(u8"\u2BC0");
ManeuverGraphic::ManeuverGraphic(ShiftText shift_val, QGraphicsItem *parent) :
    QGraphicsItem(parent),
    shift_val(shift_val)
{
    setScale(0.27);
    setAcceptHoverEvents(true);
    GraphicsItemFlags f = flags();
    f = f|ItemIsSelectable;
    setFlags(f);
    // Always start with a starting hex
    addHex(Maneuver::Stationary, HexTile::Starting_Tile);
    setVisible(false);
}

QRectF ManeuverGraphic::boundingRect() const
{
    // Use the polygon constructed in the shape() function
    return shape().boundingRect();
}

void ManeuverGraphic::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal hex_center = childItems().first()->boundingRect().width() / 2;
    qreal hex_height = childItems().first()->boundingRect().height();
    switch (shift_val) {
        case Shift_Left: hex_center = 0; break;
        case Shift_Right: hex_center = childItems().size() * hex_center; break;
        default: break;
    }

    QFont font = painter->font();
    font.setPointSize(25);
    font.setBold(true);
    font.setFamily("Tahoma");
    font.setLetterSpacing(QFont::PercentageSpacing, 110);
    painter->setFont(font);

    painter->drawText(hex_center - painter->fontMetrics().width(getID())/2, hex_height + painter->fontMetrics().height(), getID());
    painter->drawText(hex_center - painter->fontMetrics().width(getTolerances())/2, hex_height + 2*painter->fontMetrics().height(), getTolerances());
    painter->drawText(hex_center - painter->fontMetrics().width(getAdditionalInfo())/2, hex_height + 3*painter->fontMetrics().height(), getAdditionalInfo());
}

void ManeuverGraphic::addHex(Maneuver::Directions position, HexTile::TileType tile_type, qreal rotation)
{
    HexTile* tile = new HexTile(tile_type, rotation, this);
    for (Maneuver::Directions direction : directions_list) {
        moveTile(tile, direction);
    }
    moveTile(tile, position);
    directions_list << position;
}

QPainterPath ManeuverGraphic::shape() const
{
    QPainterPath path;

    // Need to translate each poly the same way the child hexes were translated
    for (auto child : childItems()) {
        QPolygon poly = static_cast<HexTile*>(child)->getHexPoly();
        poly.translate(child->pos().x(), child->pos().y());
        path.addPolygon(poly);
    }
    qreal hex_center = childItems().first()->boundingRect().width() / 2;
    switch (shift_val) {
        case Shift_Left: hex_center = 0; break;
        case Shift_Right: hex_center = childItems().size() * hex_center; break;
        default: break;
    }
    QFont font;
    font.setPointSize(25);
    font.setBold(true);
    font.setLetterSpacing(QFont::PercentageSpacing, 110);
    QFontMetrics font_metrics(font);
    QRectF rect;
    rect.setTop(path.boundingRect().bottom());
    int text_width = font_metrics.width(getTolerances());
    if (font_metrics.width(getAdditionalInfo()) > text_width) text_width = font_metrics.width(getAdditionalInfo());

    rect.setLeft(hex_center - text_width/2);
    rect.setWidth(text_width);
    rect.setHeight(3.5*font_metrics.height());
    path.addRect(rect);

    return path;
}

void ManeuverGraphic::setModelIndex(QPersistentModelIndex idx)
{
    setVisible(idx.isValid());

    // Cache the index
    maneuver_idx = idx;
    if (idx.isValid()) {
        updateManeuverState();
    }
}

void ManeuverGraphic::updateManeuverState()
{
    for (auto child : childItems()) {
        HexTile* hex = static_cast<HexTile*>(child);
        hex->setIsAvailable(maneuver_idx.sibling(maneuver_idx.row(), ManeuverItem::Can_Be_Used).data().toBool());
        hex->setWeightRestricted(maneuver_idx.sibling(maneuver_idx.row(), ManeuverItem::Is_Weight_Restricted).data().toBool());
    }
}

void ManeuverGraphic::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if (maneuver_idx.sibling(maneuver_idx.row(), ManeuverItem::IsEnabled).data().toBool()) {
        setCursor(Qt::PointingHandCursor);
        for (auto child : childItems()) {
            static_cast<HexTile*>(child)->setHovered(true);
        }
    }
    QGraphicsItem::hoverEnterEvent(event);
}

void ManeuverGraphic::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if (maneuver_idx.sibling(maneuver_idx.row(), ManeuverItem::IsEnabled).data().toBool()) {
        setCursor(Qt::ArrowCursor);
        for (auto child : childItems()) {
            static_cast<HexTile*>(child)->setHovered(false);
        }
    }
    QGraphicsItem::hoverLeaveEvent(event);
}

QVariant ManeuverGraphic::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedChange) {
        for (auto child : childItems()) {
            static_cast<HexTile*>(child)->setSelected(value.toBool());
        }
    }

    return QGraphicsItem::itemChange(change, value);
}

void ManeuverGraphic::moveTile(HexTile *tile, Maneuver::Directions position)
{
    switch (position) {
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

QString ManeuverGraphic::getID() const
{
    QString id = maneuver_idx.sibling(maneuver_idx.row(), ManeuverItem::Maneuver_Name).data().toString();
    if (!maneuver_idx.sibling(maneuver_idx.row(), ManeuverItem::Can_Be_Repeated).data().toBool()) {
        id = QString("[%1]").arg(id);
    }
    return id;
}

QString ManeuverGraphic::getTolerances() const
{
    QStringList tolerances = {maneuver_idx.sibling(maneuver_idx.row(), ManeuverItem::Climb_Value).data().toString(),
                              maneuver_idx.sibling(maneuver_idx.row(), ManeuverItem::Level_Value).data().toString(),
                              maneuver_idx.sibling(maneuver_idx.row(), ManeuverItem::Dive_Value).data().toString()};
    return tolerances.join("/");
}

QString ManeuverGraphic::getAdditionalInfo() const
{
    QString additional_info;
    if (maneuver_idx.sibling(maneuver_idx.row(), ManeuverItem::Causes_Spin_Check).data().toBool()) {
        additional_info = "(Spin Check)";
    }
    else {
        additional_info =  maneuver_idx.sibling(maneuver_idx.row(), ManeuverItem::Observer_Can_Reload).data().toBool() ? black_square : QChar();
        additional_info += maneuver_idx.sibling(maneuver_idx.row(), ManeuverItem::Can_Reload).data().toBool() ? " R" : "";
        additional_info += maneuver_idx.sibling(maneuver_idx.row(), ManeuverItem::Can_Put_Out_Fires).data().toBool() ? " F" : "";
    }
    return additional_info.trimmed();
}
