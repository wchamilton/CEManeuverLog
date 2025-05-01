#ifndef ALTCTRLSCENE_H
#define ALTCTRLSCENE_H

#include <QGraphicsScene>
#include <QPersistentModelIndex>

class ManeuverModifiers;
class AltCtrlScene : public QGraphicsScene
{
    Q_OBJECT
public:
    AltCtrlScene(QPersistentModelIndex plane_idx, QObject* parent = nullptr);
    int getCurrentAlt() const;
    void updateAltitudes(const QList<int> &alt_range, int current_alt);
    void updateTurnCounter(int turn);

public slots:
    void setManeuver(QPersistentModelIndex maneuver_idx);

private:
    QPersistentModelIndex plane_idx;
    QGraphicsTextItem* plane_name = nullptr;
    QGraphicsTextItem* plane_tolerances = nullptr;
    QGraphicsTextItem* turn_display = nullptr;
    ManeuverModifiers* panel = nullptr;
};

#endif // ALTCTRLSCENE_H
