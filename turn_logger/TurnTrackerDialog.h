#ifndef TURNTRACKERDIALOG_H
#define TURNTRACKERDIALOG_H

#include <QDialog>
#include <QPersistentModelIndex>

namespace Ui {
class TurnTrackerDialog;
}

class GameModel;
class FilterProxy;
class ManeuverScene;
class AltCtrlScene;
class FiringArcScene;

class TurnTrackerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TurnTrackerDialog(GameModel *game_model, QWidget *parent = nullptr);
    ~TurnTrackerDialog();

private slots:
    void handleTurnEnd();
    void updateAvailableAltitudes();
    void refreshCrew();
    void rotateGun(const int delta);

private:
    Ui::TurnTrackerDialog *ui;
    GameModel* game_model = nullptr;
    QSharedPointer<FilterProxy> maneuver_proxy;
    QSharedPointer<FilterProxy> crew_proxy;
    QSharedPointer<FilterProxy> turn_proxy;
    ManeuverScene* maneuver_scene = nullptr;
    AltCtrlScene* alt_ctrl_scene = nullptr;
    FiringArcScene* firing_arc_scene = nullptr;
    QPersistentModelIndex plane_idx;
};

#endif // TURNTRACKERDIALOG_H
