#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFileDialog>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include <QDebug>

#include "models/PlaneModel.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->crew_tab->addTab(new CrewControls(this), "Pilot");
    ui->engine_grp->setTitle("Engine");
    ui->wing_grp->setTitle("Wing");
    ui->fusilage_grp->setTitle("Fusilage");
    ui->tail_grp->setTitle("Tail");
    early_war_menu = ui->select_plane_menu->addMenu("Early War");
    late_war_menu = ui->select_plane_menu->addMenu("Late War");

    connect(ui->actionLoad_Planes, &QAction::triggered, this, &MainWindow::loadJSON);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadJSON()
{
    QFileDialog dlg(this);
    QFile file(dlg.getOpenFileName(this, tr("Open File"), "/home", tr("JSON files (planes.json)")));
    file.open(QIODevice::ReadOnly|QIODevice::Text);
    QJsonDocument planes_doc = QJsonDocument::fromJson(QString(file.readAll()).toUtf8());
    file.close();

    QJsonArray planes_array = planes_doc.array();
    plane_model = new PlaneModel(planes_array, this);

    ui->statusbar->showMessage(tr("%1 planes loaded").arg(plane_model->rowCount()));

    generatePlaneMenu();
}

void MainWindow::testConnect()
{
    QPersistentModelIndex idx = static_cast<QAction*>(sender())->data().toPersistentModelIndex();
    qDebug() << idx.data().toString();
}

void MainWindow::generatePlaneMenu()
{
    for (int i=0; i<plane_model->rowCount(); ++i) {
        QString era = plane_model->index(i, PlaneItem::Plane_Era).data().toString();
        if (era == "Early War") {
            QPersistentModelIndex idx(plane_model->index(i, PlaneItem::Plane_Name));
            QAction* plane_action = early_war_menu->addAction(idx.data().toString());
            plane_action->setData(idx);
            connect(plane_action, &QAction::triggered, this, &MainWindow::testConnect);
        }
        else if (era == "Late War") {
            QPersistentModelIndex idx(plane_model->index(i, PlaneItem::Plane_Name));
            QAction* plane_action = late_war_menu->addAction(idx.data().toString());
            plane_action->setData(idx);
            connect(plane_action, &QAction::triggered, this, &MainWindow::testConnect);
        }
    }
}
