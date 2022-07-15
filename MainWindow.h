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
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void loadJSON(QString file_path);
    void setSelectedPlane();

private:
    void generatePlaneMenu();

    Ui::MainWindow *ui;
    PlaneModel* plane_model = nullptr;
    PlaneFilterProxy* maneuver_proxy_model = nullptr;
    PlaneFilterProxy* crew_proxy_model = nullptr;
    ManeuverScene* maneuver_scene = nullptr;
    QMenu* early_war_menu;
    QMenu* late_war_menu;
};
#endif // MAINWINDOW_H
