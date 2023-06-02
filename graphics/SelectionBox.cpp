#include "SelectionBox.h"
#include <QPen>
#include <QPainter>
#include <QCursor>
#include <QDebug>
#include <QGraphicsSceneHoverEvent>

SelectionBox::SelectionBox(QGraphicsItem *parent) : QGraphicsItem(parent)
{
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setAcceptHoverEvents(true);

    up_arrow.poly << QPoint(boundingRect().width()/2, 7)
             << QPoint(boundingRect().right() - 7, boundingRect().height()/4+5)
             << QPoint(7, boundingRect().height()/4+5)
             << QPoint(boundingRect().width()/2, 7);
    up_arrow.fill = QColor(100, 200, 255);

    down_arrow.poly << QPoint(boundingRect().width()/2, boundingRect().height() - 7)
               << QPoint(boundingRect().right() - 7, 3*boundingRect().height()/4-5)
               << QPoint(7, 3*boundingRect().height()/4-5)
               << QPoint(boundingRect().width()/2, boundingRect().height() - 7);
    down_arrow.fill = QColor(255, 100, 100);
}

QRectF SelectionBox::boundingRect() const
{
    return QRectF(0, 0, 50, 120);
}

void SelectionBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    QPen pen;
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);

    auto drawArrowBtn = [=] (ArrowBtn arrow) {
        QPen old_pen(painter->pen());
        if (arrow.hovered) {
            QPen temp_pen(old_pen);
            temp_pen.setWidth(3);
            painter->setPen(temp_pen);
        }

        QPainterPath path;
        path.addPolygon(arrow.poly);
        QColor color = Qt::white;
        if (arrow.hovered || arrow.force_filled) {
            color = arrow.fill;
        }
        painter->fillPath(path, color);
        painter->drawPolygon(arrow.poly);
        painter->setPen(old_pen);
    };

    drawArrowBtn(up_arrow);
    drawArrowBtn(down_arrow);

    QFont font = painter->font();
    font.setPixelSize(48);
    font.setFamily("Arial");
    painter->setFont(font);
    painter->drawText(boundingRect(), Qt::AlignCenter, QString::number(digits[selected_digit_idx]));

    // Border
    pen.setWidth(5);
    painter->setPen(pen);
    painter->drawRoundedRect(boundingRect(), 10, 10);
}

void SelectionBox::setDigits(QList<int> arg)
{
    digits = arg;
    up_arrow.force_filled = false;
    down_arrow.force_filled = false;
}

void SelectionBox::setSelectedDigit(int selected_digit)
{
    selected_digit_idx = digits.indexOf(selected_digit);
    starting_digit_idx = digits.indexOf(selected_digit);
}

void SelectionBox::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    up_arrow.hovered = up_arrow.poly.containsPoint(event->pos().toPoint(), Qt::OddEvenFill);
    down_arrow.hovered = down_arrow.poly.containsPoint(event->pos().toPoint(), Qt::OddEvenFill);
    setCursor(up_arrow.hovered || down_arrow.hovered ? Qt::PointingHandCursor : Qt::ArrowCursor);

    update();
    QGraphicsItem::hoverMoveEvent(event);
}

void SelectionBox::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (!up_arrow.poly.containsPoint(event->pos().toPoint(), Qt::OddEvenFill) &&
        !down_arrow.poly.containsPoint(event->pos().toPoint(), Qt::OddEvenFill)) {
        QGraphicsItem::mousePressEvent(event);
        return;
    }

    if (selected_digit_idx < digits.size()-1 && up_arrow.poly.containsPoint(event->pos().toPoint(), Qt::OddEvenFill)) {
        ++selected_digit_idx;
    }
    else if (selected_digit_idx > 0 && down_arrow.poly.containsPoint(event->pos().toPoint(), Qt::OddEvenFill)) {
        --selected_digit_idx;
    }

    up_arrow.force_filled = selected_digit_idx > starting_digit_idx;
    down_arrow.force_filled = selected_digit_idx < starting_digit_idx;

    update();
    QGraphicsItem::mousePressEvent(event);
}
