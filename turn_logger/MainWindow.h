#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonDocument>

#include "PlanePartDamageTracker.h"
#include "CrewControls.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class PlaneModel;
class PlaneFilterProxy;
class ManeuverScene;
class QActionGroup;
class QComboBox;
class QTreeWidgetItem;
class QPushButton;
class TurnModel;
class TurnFilterProxy;
class FiringArcScene;
class AltCtrlScene;
class QLabel;
class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum TurnState {
        Start_Of_Turn = 0,
        Movement_Selected,
        Movement_Locked
    };

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void loadJSON(QString file_path);
    void setSelectedPlane();
    void rotateSelectedFlexibleGun(int delta);
    void handleTurnEnd();

private:
    void setTurnState(TurnState state);
    void autoLoadPlanes();
    void clearUI();
    void generatePlaneMenu(QPersistentModelIndex idx);

    Ui::MainWindow *ui;
    PlaneModel* plane_model = nullptr;
    TurnModel* turn_model = nullptr;
    TurnFilterProxy* turn_proxy = nullptr;
    PlaneFilterProxy* maneuver_proxy_model = nullptr;
    PlaneFilterProxy* crew_proxy_model = nullptr;
    ManeuverScene* maneuver_scene = nullptr;
    AltCtrlScene* alt_ctrl_scene = nullptr;
    FiringArcScene* firing_arc_scene = nullptr;
    QMenu* early_war_menu = nullptr;
    QMenu* late_war_menu = nullptr;
    QActionGroup* plane_action_group = nullptr;
    QMap<int, CrewControls*> crew_control_widgets;
    QLabel* status_bar_label = nullptr;
};
#endif // MAINWINDOW_H
