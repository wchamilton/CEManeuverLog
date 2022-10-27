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
class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum TurnCols {
        Maneuver_Name = 0,
        Altitude,
        Fuel_Used
    };

    enum CrewTurnCols {
        Crew_Name = 0,
        Crew_Action
    };

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void loadJSON(QString file_path);
    void setSelectedPlane();
    void updateCurrentTurn();
    void addTurn();

private:
    void autoLoadPlanes();
    void clearUI();
    void generatePlaneMenu(QPersistentModelIndex idx);
    int calculateFuelUsed();
    void setAvailableAltitudes();
    void setAvailableManeuvers();

    Ui::MainWindow *ui;
    PlaneModel* plane_model = nullptr;
    PlaneFilterProxy* maneuver_proxy_model = nullptr;
    PlaneFilterProxy* crew_proxy_model = nullptr;
    ManeuverScene* maneuver_scene = nullptr;
    QMenu* early_war_menu = nullptr;
    QMenu* late_war_menu = nullptr;
    QActionGroup* plane_action_group = nullptr;
    QPushButton* next_turn_btn = nullptr;
    QComboBox* alt_cmb = nullptr;
    QTreeWidgetItem* current_turn_item = nullptr;
    QMap<QString, QTreeWidgetItem*> current_crew_turn_items;
    QMap<QString, CrewControls*> crew_control_widgets;
    QTreeWidgetItem* next_turn_btn_item = nullptr;
};
#endif // MAINWINDOW_H
