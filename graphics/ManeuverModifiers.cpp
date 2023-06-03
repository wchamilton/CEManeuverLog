#include "ManeuverModifiers.h"

#include <QPainter>
#include <QGraphicsSvgItem>
#include <QDebug>

#include "SelectionBox.h"
#include "models/PlaneItems.h"
#include "models/TurnModel.h"

ManeuverModifiers::ManeuverModifiers(QGraphicsItem *parent) : QGraphicsItem(parent)
{
    setFlag(ItemIsFocusable, true);
    setFiltersChildEvents(true);

    digit_box = new SelectionBox(this);
    digit_box->setPos(10, 10); // -3 to account for the border

    plane_profile = new QGraphicsSvgItem("./graphics/plane_profile.svg", this);
//    plane_profile = new QGraphicsSvgItem("../CEManeuverLog/graphics/plane_profile.svg", this);
    plane_profile->setScale(0.15);
    plane_profile->setPos(-160, -110);
    plane_profile->setTransformOriginPoint(plane_profile->boundingRect().width()/2, plane_profile->boundingRect().height()/2);
}

QRectF ManeuverModifiers::boundingRect() const
{
    return QRectF(0, 0, 170, digit_box->boundingRect().height() + 20);
}

void ManeuverModifiers::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen pen;
    pen.setWidth(3);
    painter->setPen(pen);
    painter->drawRoundedRect(boundingRect(), 10, 10);

    QFont font = painter->font();
    font.setPixelSize(24);
    painter->setFont(font);

    if (!turn_model || !current_maneuver.isValid()) {
        painter->drawText(boundingRect(), Qt::AlignCenter, QString("Select a\nmaneuver"));
    }
    else {
        QFontMetrics metrics(font);
        painter->drawText(70, boundingRect().height()-20, constructManeuverText());
    }
}

void ManeuverModifiers::setTurnModel(TurnModel *model)
{
    turn_model = model;
    setCurrentAltitude(model->getStartingAlt());
}

void ManeuverModifiers::setCurrentManeuver(QPersistentModelIndex maneuver)
{
    current_maneuver = maneuver;
    plane_profile->setVisible(maneuver.isValid());
    digit_box->setVisible(maneuver.isValid());
    update();
}

int ManeuverModifiers::getCurrentAltitude() const
{
    return digit_box->getSelectedDigit();
}

void ManeuverModifiers::setCurrentAltitude(int arg)
{
    selected_altitude = arg;
    digit_box->setSelectedDigit(arg);
}

void ManeuverModifiers::setAvailableAltitudes(QList<int> altitudes)
{
    digit_box->setDigits(altitudes);
}

void ManeuverModifiers::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem* item = focusItem();
    if (item != nullptr && item != this && childItems().contains(item)) {
        selected_altitude = static_cast<SelectionBox*>(item)->getSelectedDigit();
        update();
    }

    QGraphicsItem::mousePressEvent(event);
}

QString ManeuverModifiers::constructManeuverText()
{
    QString maneuver_name = current_maneuver.sibling(current_maneuver.row(), ManeuverItem::Maneuver_Name).data().toString();
    QModelIndex prev_turn_idx = turn_model->lastIndex(TurnItem::Turn_Altitude_Col);
    int prev_alt = prev_turn_idx.isValid() ? prev_turn_idx.data(Qt::UserRole).toInt() : turn_model->getStartingAlt();

    auto getDirectionTag = [=](int column){
        return current_maneuver.sibling(current_maneuver.row(), column).data().toString();
    };
    QString direction_tag;
    if (prev_alt > selected_altitude) {
        direction_tag = getDirectionTag(ManeuverItem::Dive_Value);
        plane_profile->setRotation(prev_alt - selected_altitude > 1 ? 45 : 26);
    }
    else if (prev_alt < selected_altitude) {
        direction_tag = getDirectionTag(ManeuverItem::Climb_Value);
        plane_profile->setRotation(selected_altitude - prev_alt > 1 ? -45 : -26);
    }
    else {
        direction_tag = getDirectionTag(ManeuverItem::Level_Value);
        plane_profile->setRotation(0);
    }
    return maneuver_name + "-" + direction_tag;
}
