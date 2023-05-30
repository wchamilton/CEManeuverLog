#ifndef MANEUVERMODIFIERS_H
#define MANEUVERMODIFIERS_H

#include <QGraphicsItem>
#include <QPersistentModelIndex>

class TurnModel;
class SelectionBox;
class QGraphicsSvgItem;
class ManeuverModifiers : public QGraphicsItem
{
public:
    ManeuverModifiers(QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void setTurnModel(TurnModel* model);
    void setCurrentManeuver(QPersistentModelIndex maneuver);
    int getCurrentAltitude() const;
    void setCurrentAltitude(int arg);
    void setAvailableAltitudes(QList<int> altitudes);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QString constructManeuverText();

    QMap<int, bool> available_altitudes;
    int selected_altitude = 0;
    QPersistentModelIndex current_maneuver;
    TurnModel* turn_model = nullptr;

    SelectionBox* digit_box = nullptr;
    QGraphicsSvgItem* plane_profile = nullptr;
};

#endif // MANEUVERMODIFIERS_H
