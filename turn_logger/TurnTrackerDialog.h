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
class TurnHistoryDialog;
class StatusEffectsInterface;

class TurnTrackerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TurnTrackerDialog(GameModel *game_model, QWidget *parent = nullptr);
    ~TurnTrackerDialog();

private slots:
    void handleTurnEnd();
    void handleManeuverSelection(QModelIndex maneuver_idx);
    int calculateFuelUsage();

private:
    void updateAvailableManeuvers();

    Ui::TurnTrackerDialog *ui;
    GameModel* game_model = nullptr;
    QSharedPointer<FilterProxy> maneuver_proxy;
    QSharedPointer<FilterProxy> crew_proxy;
    QSharedPointer<FilterProxy> turn_proxy;
    QSharedPointer<FilterProxy> effects_proxy;
    ManeuverScene* maneuver_scene = nullptr;
    AltCtrlScene* alt_ctrl_scene = nullptr;
    TurnHistoryDialog* turn_log_view = nullptr;
    StatusEffectsInterface* effects_interface = nullptr;
    QPersistentModelIndex plane_idx;
    QPersistentModelIndex pilot_idx;
};

#endif // TURNTRACKERDIALOG_H
