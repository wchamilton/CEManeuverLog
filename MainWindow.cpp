#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFileDialog>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include <QDebug>
#include <QDesktopWidget>
#include <QSettings>

#include "models/PlaneModel.h"
#include "graphics/ManeuverScene.h"
#include "PlaneEditor.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->actionQuit->setShortcut(Qt::ControlModifier+Qt::Key_Q);

    // Init the models
    plane_model = new PlaneModel(this);

    maneuver_proxy_model = new PlaneFilterProxy(plane_model, this);
    maneuver_proxy_model->expandFilter(BaseItem::Maneuver_Item_Type);
    ui->maneuver_cmb->setModel(maneuver_proxy_model);

    crew_proxy_model = new PlaneFilterProxy(plane_model, this);
    crew_proxy_model->expandFilter(BaseItem::Crew_Item_Type);
    crew_proxy_model->expandFilter(BaseItem::Gun_Item_Type);

    // Set up the damage trackers
    ui->engine_grp->setTitle("Engine");
    ui->wing_grp->setTitle("Wing");
    ui->fuselage_grp->setTitle("Fuselage");
    ui->tail_grp->setTitle("Tail");

    early_war_menu = ui->select_plane_menu->addMenu("Early War");
    late_war_menu = ui->select_plane_menu->addMenu("Late War");

    maneuver_scene = new ManeuverScene(ui->graphicsView);
    maneuver_scene->positionManeuvers();
    ui->graphicsView->setScene(maneuver_scene);
    ui->graphicsView->setRenderHints(QPainter::Antialiasing);

    // Load the planes if a location has been already set
    autoLoadPlanes();
    ui->maneuver_cmb->setCurrentIndex(-1);

    connect(ui->actionLoad_Planes, &QAction::triggered, this, [&]{
        QString file_path = QFileDialog::getOpenFileName(this, tr("Open File"), "/home", tr("JSON files (*.json)"));
        loadJSON(file_path);
    });

    connect(ui->actionCreate_plane, &QAction::triggered, this, [&]{
        PlaneEditor(this).exec();
    });

    connect(ui->actionEdit_plane, &QAction::triggered, this, [&] {
        QString file_path = QFileDialog::getOpenFileName(this, tr("Open File"), "/home", tr("JSON files (*.json)"));
        if (file_path != "") {
            QFile file(file_path);
            if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
                qWarning() << "Could not open" << file_path;
                return;
            }
            QJsonDocument planes_doc = QJsonDocument::fromJson(QString(file.readAll()).toUtf8());
            file.close();
            PlaneEditor(planes_doc.object(), this).exec();
        }
    });

    connect(ui->actionSet_Plane_Auto_Load_Location, &QAction::triggered, this, [&] {
        QSettings settings;
        QString load_location = "/home";
        if (settings.contains("auto-load_location")) {
            load_location = settings.value("auto-load_location").toString();
        }
        load_location = QFileDialog::getExistingDirectory(this, tr("Select Folder"), load_location);
        if (load_location != "") {
            settings.setValue("auto-load_location", load_location);
            autoLoadPlanes();
        }
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

    generatePlaneMenu(plane_model->loadPlaneJSON(planes_doc.object()));
    ui->statusbar->showMessage(tr("%1 planes loaded").arg(plane_model->rowCount()));
}

void MainWindow::setSelectedPlane()
{
    ui->turn_controls_grp->setEnabled(true);

    QPersistentModelIndex plane_idx = static_cast<QAction*>(sender())->data().toPersistentModelIndex();
    maneuver_scene->setManeuvers(maneuver_proxy_model->mapFromSource(plane_idx));
    maneuver_scene->update();
    ui->maneuver_cmb->setEnabled(true);
    ui->maneuver_cmb->setRootModelIndex(maneuver_proxy_model->mapFromSource(plane_idx));
    ui->maneuver_cmb->setCurrentIndex(-1);

    // Populate the crew tab and the gun tabs for each crew
    ui->crew_tab->clear();
    ui->gun_tab->clear();
    for (int i=0; i<crew_proxy_model->rowCount(crew_proxy_model->mapFromSource(plane_idx)); ++i) {
        QPersistentModelIndex crew_idx = crew_proxy_model->index(i, CrewItem::Crew_Role, crew_proxy_model->mapFromSource(plane_idx));
        ui->crew_tab->addTab(new CrewControls(ui->crew_tab), crew_idx.data().toString());

        for (int j=0; j<crew_proxy_model->rowCount(crew_idx); ++j) {
            QPersistentModelIndex gun_idx = crew_proxy_model->index(j, GunItem::Gun_Name, crew_idx);
            QString gun_prefix = "";
            int gun_count = gun_idx.sibling(gun_idx.row(), GunItem::Gun_Count).data().toInt();
            if (gun_count == 2) {
                gun_prefix = "Twin ";
            }
            else if (gun_count == 3) {
                gun_prefix = "Triple ";
            }
            ui->gun_tab->addTab(new GunControls(gun_idx, ui->gun_tab),
                                QString("%1's %2%3").arg(crew_idx.data().toString()).arg(gun_prefix).arg(gun_idx.data().toString()));
        }
    }

    // Set the module HP indexes
    ui->engine_grp->setModelIndexes(plane_idx.sibling(plane_idx.row(), PlaneItem::Engine_HP), plane_idx.sibling(plane_idx.row(), PlaneItem::Engine_Critical));
    ui->wing_grp->setModelIndexes(plane_idx.sibling(plane_idx.row(), PlaneItem::Wing_HP), plane_idx.sibling(plane_idx.row(), PlaneItem::Wing_Critical));
    ui->fuselage_grp->setModelIndexes(plane_idx.sibling(plane_idx.row(), PlaneItem::Fuselage_HP), plane_idx.sibling(plane_idx.row(), PlaneItem::Fuselage_Critical));
    ui->tail_grp->setModelIndexes(plane_idx.sibling(plane_idx.row(), PlaneItem::Tail_HP), plane_idx.sibling(plane_idx.row(), PlaneItem::Tail_Critical));
}

void MainWindow::autoLoadPlanes()
{
    QSettings settings;
    QString load_location = settings.contains("auto-load_location") ? settings.value("auto-load_location").toString() : "";
    if (load_location != "") {
        clearUI();
        QDir planes_dir(load_location);
        for (QFileInfo file_info : planes_dir.entryInfoList({"*.json"})) {
            loadJSON(file_info.absoluteFilePath());
        }
    }
}

void MainWindow::clearUI()
{
    plane_model->clearModel();

    early_war_menu->clear();
    late_war_menu->clear();

    ui->engine_grp->clear();
    ui->wing_grp->clear();
    ui->fuselage_grp->clear();
    ui->tail_grp->clear();
    ui->crew_tab->clear();
    ui->gun_tab->clear();

    maneuver_scene->setManeuvers(QPersistentModelIndex());
}

void MainWindow::generatePlaneMenu(QPersistentModelIndex idx)
{
    QAction* plane_action = nullptr;
    if (idx.sibling(idx.row(), PlaneItem::Plane_Era).data().toString() == "Early War") {
        plane_action = early_war_menu->addAction(idx.data().toString());
    }
    else if (idx.sibling(idx.row(), PlaneItem::Plane_Era).data().toString() == "Late War") {
        plane_action = late_war_menu->addAction(idx.data().toString());
    }
    plane_action->setData(idx);
    connect(plane_action, &QAction::triggered, this, &MainWindow::setSelectedPlane);
}
