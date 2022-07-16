#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFileDialog>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include <QDebug>
#include <QDesktopWidget>

#include "models/PlaneModel.h"
#include "graphics/ManeuverScene.h"
#include "PlaneEditor.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->engine_grp->setTitle("Engine");
    ui->wing_grp->setTitle("Wing");
    ui->fusilage_grp->setTitle("Fusilage");
    ui->tail_grp->setTitle("Tail");

    // Test code
    ui->gun_tab->addTab(new GunControls(ui->gun_tab), "Pilot Gun");

    early_war_menu = ui->select_plane_menu->addMenu("Early War");
    late_war_menu = ui->select_plane_menu->addMenu("Late War");
    loadJSON("../planes/Albratross_DI-DII.json");

    maneuver_scene = new ManeuverScene(ui->graphicsView);
    maneuver_scene->positionManeuvers();
    ui->graphicsView->setScene(maneuver_scene);
    ui->graphicsView->setRenderHints(QPainter::Antialiasing);

    connect(ui->actionLoad_Planes, &QAction::triggered, this, [&]{
        QString file_path = QFileDialog::getOpenFileName(this, tr("Open File"), "/home", tr("JSON files (planes.json)"));
        loadJSON(file_path);
    });

    connect(ui->actionPlane_Editor, &QAction::triggered, this, [&]{
        PlaneEditor editor;
        editor.exec();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadJSON(QString file_path)
{
    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        qWarning() << "Could not open" << file_path;
        return;
    }
    QJsonDocument planes_doc = QJsonDocument::fromJson(QString(file.readAll()).toUtf8());
    file.close();

    delete plane_model;
    delete maneuver_proxy_model;
    delete crew_proxy_model;

    plane_model = new PlaneModel(this);
    plane_model->loadPlaneJSON(planes_doc.object());

    maneuver_proxy_model = new PlaneFilterProxy(plane_model, this);
    maneuver_proxy_model->setTypeFilter(BaseItem::Maneuver_Item_Type);
    ui->maneuver_cmb->setModel(maneuver_proxy_model);
    ui->maneuver_cmb->setCurrentIndex(-1);

    crew_proxy_model = new PlaneFilterProxy(plane_model, this);
    crew_proxy_model->setTypeFilter(BaseItem::Crew_Item_Type);

    ui->statusbar->showMessage(tr("%1 planes loaded").arg(plane_model->rowCount()));
    early_war_menu->clear();
    late_war_menu->clear();
    generatePlaneMenu();
}

void MainWindow::setSelectedPlane()
{
    ui->turn_controls_grp->setEnabled(true);

    QPersistentModelIndex plane_idx = static_cast<QAction*>(sender())->data().toPersistentModelIndex();
    maneuver_scene->setManeuvers(maneuver_proxy_model->mapFromSource(plane_idx));
    maneuver_scene->update();
    ui->maneuver_cmb->setEnabled(true);
    ui->maneuver_cmb->setRootModelIndex(maneuver_proxy_model->mapFromSource(plane_idx));

    ui->crew_tab->clear();
    for (int i=0; i<crew_proxy_model->rowCount(crew_proxy_model->mapFromSource(plane_idx)); ++i) {
        QPersistentModelIndex crew_idx = crew_proxy_model->index(i, CrewItem::Crew_Role, crew_proxy_model->mapFromSource(plane_idx));
        ui->crew_tab->addTab(new CrewControls(this), crew_idx.data().toString());
    }
}

void MainWindow::generatePlaneMenu()
{
    for (int i=0; i<plane_model->rowCount(); ++i) {
        QString era = plane_model->index(i, PlaneItem::Plane_Era).data().toString();
        QPersistentModelIndex idx(plane_model->index(i, PlaneItem::Plane_Name));
        if (era == "Early War") {
            QAction* plane_action = early_war_menu->addAction(idx.data().toString());
            plane_action->setData(idx);
            connect(plane_action, &QAction::triggered, this, &MainWindow::setSelectedPlane);
        }
        else if (era == "Late War") {
            QAction* plane_action = late_war_menu->addAction(idx.data().toString());
            plane_action->setData(idx);
            connect(plane_action, &QAction::triggered, this, &MainWindow::setSelectedPlane);
        }
    }
}
