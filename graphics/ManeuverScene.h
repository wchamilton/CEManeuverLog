#ifndef MANEUVERSCENE_H
#define MANEUVERSCENE_H

#include <QGraphicsScene>
#include <QPersistentModelIndex>
#include <QMap>

class ManeuverGraphic;
class ManeuverScene : public QGraphicsScene
{
    Q_OBJECT
public:
    ManeuverScene(QPersistentModelIndex filtered_plane_idx, QObject *parent = nullptr);
    ManeuverGraphic* getManeuver(QString maneuver_name);
    QPersistentModelIndex getSelectedManeuverIdx();
    QString getSelectedManeuver();
    void addManeuver(QPersistentModelIndex maneuver_idx);
    void removeManeuver(QPersistentModelIndex maneuver_idx);
    void setManeuver(QPersistentModelIndex maneuver_idx);
    void updateManeuver(QString id);
    void clearSelection();
    void applyScheduleBG();
    void positionManeuvers();

signals:
    void maneuverClicked(QPersistentModelIndex maneuver_idx);

private slots:
    void handleFocusChanges(QGraphicsItem* newFocusItem, QGraphicsItem* oldFocusItem, Qt::FocusReason reason);

private:
    QMap<QString, ManeuverGraphic*> maneuver_map;
    ManeuverGraphic* selected_maneuver = nullptr;
    QGraphicsPixmapItem* background_item = nullptr;
    QPersistentModelIndex filtered_plane_idx;
    int selected_altitude = 0;
};

#endif // MANEUVERSCENE_H
