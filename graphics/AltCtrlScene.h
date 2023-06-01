#ifndef ALTCTRLSCENE_H
#define ALTCTRLSCENE_H

#include <QGraphicsScene>
#include <QPersistentModelIndex>

class ManeuverModifiers;
class TurnModel;
class AltCtrlScene : public QGraphicsScene
{
    Q_OBJECT
public:
    AltCtrlScene(QObject* parent = nullptr);
    void setPlane(QPersistentModelIndex plane_idx);
    void setTurnModel(TurnModel* model);
    int getCurrentAlt() const;

public slots:
    void setManeuver(QPersistentModelIndex maneuver_idx);

private:
    void calculateAvailableAltitudes(QPersistentModelIndex current_maneuver);

    QPersistentModelIndex plane_idx;
    QGraphicsTextItem* plane_name = nullptr;
    QGraphicsTextItem* plane_tolerances = nullptr;
    ManeuverModifiers* panel = nullptr;
    TurnModel* turn_model = nullptr;
//    int selected_altitude = 0;
};

#endif // ALTCTRLSCENE_H
