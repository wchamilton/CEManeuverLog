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
    ManeuverScene(QAbstractItemModel *model, QPersistentModelIndex filtered_plane_idx, QObject *parent = nullptr);
    ManeuverGraphic* getManeuver(QString maneuver_name);
    QPersistentModelIndex getSelectedManeuverIdx();
    QString getSelectedManeuver();
    void addManeuver(QPersistentModelIndex maneuver_idx);
    void removeManeuver(QPersistentModelIndex maneuver_idx);
    void setManeuver(QPersistentModelIndex maneuver_idx);
    void updateManeuver(QString id);
    void clearSelection();

signals:
    void maneuverClicked(QPersistentModelIndex maneuver_idx);

private slots:
    void handleFocusChanges(QGraphicsItem* newFocusItem, QGraphicsItem* oldFocusItem, Qt::FocusReason reason);

private:
    void applyScheduleBG();

    QMap<QString, ManeuverGraphic*> maneuver_map;
    ManeuverGraphic* selected_maneuver = nullptr;
    QGraphicsPixmapItem* background_item = nullptr;
};

#endif // MANEUVERSCENE_H
