#ifndef MANEUVERSCENE_H
#define MANEUVERSCENE_H

#include <QObject>
#include <QGraphicsScene>
#include <QMap>

class ManeuverGraphic;
class ManeuverScene : public QGraphicsScene
{
    Q_OBJECT
public:
    ManeuverScene(QObject *parent = nullptr);
    ManeuverGraphic* getManeuver(QString maneuver_name);
    void addManeuver(QPersistentModelIndex maneuver_idx);
    void removeManeuver(QPersistentModelIndex maneuver_idx);
    void setManeuver(QPersistentModelIndex maneuver_idx);
    void setManeuvers(QPersistentModelIndex plane_idx);
    void updateManeuver(QString id);
    void positionManeuvers();

signals:
    void maneuverSelectionChanged(QString maneuver);

private:
    QMap<QString, ManeuverGraphic*> maneuver_map;
};

#endif // MANEUVERSCENE_H
