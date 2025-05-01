#ifndef PLANEEDITOR_H
#define PLANEEDITOR_H

#include <QDialog>
#include <QPersistentModelIndex>

namespace Ui {
class PlaneEditor;
}

class QListWidgetItem;
class GameModel;
class FilterProxy;
class ManeuverScene;
class PlaneEditor : public QDialog
{
    Q_OBJECT

public:
    explicit PlaneEditor(QWidget *parent = nullptr);
    explicit PlaneEditor(GameModel *game_model, QPersistentModelIndex plane_idx, QWidget *parent = nullptr);
    ~PlaneEditor();

private slots:
    void handleManeuverCheck(QListWidgetItem* item);
    void handleManeuverListContextMenu(const QPoint &pos);
    void handleManeuverPropertyChanges(const QVariant &arg);
    void exportJSON();

private:
    void init();

    Ui::PlaneEditor *ui;
    GameModel* game_model = nullptr;
    FilterProxy* crew_proxy = nullptr;
    FilterProxy* maneuver_proxy = nullptr;
    ManeuverScene* maneuver_scene = nullptr;
    QPersistentModelIndex plane_idx;
    QMap<QString, QPersistentModelIndex> maneuver_idx_map;
};

#endif // PLANEEDITOR_H
