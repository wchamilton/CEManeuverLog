#ifndef SELECTIONBOX_H
#define SELECTIONBOX_H

#include <QGraphicsItem>

class SelectionBox : public QGraphicsItem
{
    struct ArrowBtn {
        QPolygon poly;
        QColor fill = Qt::white;
        bool hovered = false;
        bool force_filled = false;
    };

public:
    SelectionBox(QGraphicsItem *parent = nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void setDigits(QList<int> arg);
    int getSelectedDigit() { return digits[selected_digit_idx]; }
    void setSelectedDigit(int selected_digit);

protected:
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    int starting_digit_idx = 0;
    int selected_digit_idx = 0;
    QList<int> digits = {0,1,2,3,4,5,6,7,8,9}; // Initialize 0-9 just so we have something
    ArrowBtn up_arrow;
    ArrowBtn down_arrow;
};

#endif // SELECTIONBOX_H
