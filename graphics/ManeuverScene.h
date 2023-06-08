#ifndef MANEUVERSCENE_H
#define MANEUVERSCENE_H

#include <QObject>
#include <QGraphicsScene>
#include <QMap>
#include <QPersistentModelIndex>

class ManeuverGraphic;
class PlaneFilterProxy;
class TurnModel;
class ManeuverScene : public QGraphicsScene
{
    Q_OBJECT
public:
    ManeuverScene(PlaneFilterProxy* maneuver_proxy, QObject *parent = nullptr);
    ManeuverGraphic* getManeuver(QString maneuver_name);
    QPersistentModelIndex getSelectedManeuverIdx();
    QString getSelectedManeuver();
    void addManeuver(QPersistentModelIndex maneuver_idx);
    void removeManeuver(QPersistentModelIndex maneuver_idx);
    void setManeuver(QPersistentModelIndex maneuver_idx);
    void setPlane(QPersistentModelIndex plane_idx);
    void setTurnModel(TurnModel* model);
    void updateManeuver(QString id);
    void clearSelection();
    void applyScheduleBG();
    void positionManeuvers();
    void setManeuversAvailable(QPersistentModelIndex pilot_idx);

signals:
    void maneuverClicked(QPersistentModelIndex maneuver_idx);

private slots:
    void handleFocusChanges(QGraphicsItem* newFocusItem, QGraphicsItem* oldFocusItem, Qt::FocusReason reason);

private:
    QMap<QString, ManeuverGraphic*> maneuver_map;
    ManeuverGraphic* selected_maneuver = nullptr;
    QGraphicsPixmapItem* background_item;
    QPersistentModelIndex plane_idx;
    PlaneFilterProxy* maneuver_proxy_model = nullptr;
    TurnModel* turn_model = nullptr;
    int selected_altitude = 0;
};

#endif // MANEUVERSCENE_H
