#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonDocument>

#include "PlanePartDamageTracker.h"
#include "CrewControls.h"
#include "GunControls.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class PlaneModel;
class PlaneFilterProxy;
class ManeuverScene;
class QActionGroup;
class MainWindow : public QMainWindow
{
    Q_OBJECT

    struct CrewAction {
        QString name;
        QString action_taken;
        int shots_fired;
    };

    struct TurnData {
        QString maneuver;
        int alt;
        int fuel_used;
        QList<CrewAction> crew_actions;
    };

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void loadJSON(QString file_path);
    void setSelectedPlane();
    void logTurn();

private:
    void autoLoadPlanes();
    void clearUI();
    void generatePlaneMenu(QPersistentModelIndex idx);
    int calculateFuelUsed();
    void setAvailableAltitudes();

    Ui::MainWindow *ui;
    PlaneModel* plane_model = nullptr;
    PlaneFilterProxy* maneuver_proxy_model = nullptr;
    PlaneFilterProxy* crew_proxy_model = nullptr;
    ManeuverScene* maneuver_scene = nullptr;
    QMenu* early_war_menu = nullptr;
    QMenu* late_war_menu = nullptr;
    QActionGroup* plane_action_group = nullptr;
    QList<TurnData> turn_history;
};
#endif // MAINWINDOW_H
