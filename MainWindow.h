#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "PlanePartDamageTracker.h"
#include "CrewControls.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class PlaneModel;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void loadJSON();

private slots:
    void testConnect();

private:
    void generatePlaneMenu();

    Ui::MainWindow *ui;
    PlaneModel* plane_model = nullptr;
    QMenu* early_war_menu;
    QMenu* late_war_menu;
};
#endif // MAINWINDOW_H
