#include "EntryDialog.h"
#include "models/GameModel.h"
#include "ui_EntryDialog.h"
#include "CEManeuvers.h"
#include "editor/PlaneEditor.h"
#include "turn_logger/PlaneSelectionDialog.h"

#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

EntryDialog::EntryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EntryDialog)
{
    ui->setupUi(this);
    auto setColour = [=](QPushButton* button, QColor colour) {
        QPalette btnPalette;
        btnPalette.setBrush(QPalette::Button, colour);
        button->setAutoFillBackground(true);
        button->setPalette(btnPalette);
        button->update();
    };
    setColour(ui->newPlaneBtn, QColor(0x4ab589));
    setColour(ui->editPlaneBtn, QColor(0xB54A76));
    setColour(ui->newMatchBtn, QColor(0xD1A72E));
    setColour(ui->loadMatchBtn, QColor(0x2E58D1));

    connect(ui->newPlaneBtn, &QPushButton::clicked, this, [=](){
        PlaneEditor(this).exec();
    });
    connect(ui->exitBtn, &QPushButton::clicked, this, [=](){
        QApplication::quit();
    });

    connect(ui->editPlaneBtn, &QPushButton::clicked, this, &EntryDialog::editPlaneAction);
    connect(ui->newMatchBtn, &QPushButton::clicked, this, &EntryDialog::newMatchAction);
}

EntryDialog::~EntryDialog()
{
    delete ui;
}

void EntryDialog::editPlaneAction()
{
    QFileDialog fileDlg(this, tr("Open File"), PLANES_LOCATION, tr("JSON files (*.json)"));
    fileDlg.setOption(QFileDialog::DontUseNativeDialog, true);
    fileDlg.setFileMode(QFileDialog::ExistingFile);
    fileDlg.setViewMode(QFileDialog::Detail);

    if (fileDlg.exec() && !fileDlg.selectedFiles().isEmpty()) {
        QFile file(fileDlg.selectedFiles().constFirst());
        if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
            qWarning() << "Could not open" << file.fileName();
            return;
        }
        QJsonDocument planes_doc = QJsonDocument::fromJson(QString(file.readAll()).toUtf8());
        file.close();
        PlaneEditor(planes_doc.object(), this).exec();
    }
}

void EntryDialog::newMatchAction()
{
    QSharedPointer<GameModel> game_model = QSharedPointer<GameModel>::create(new GameModel());
    if (PlaneSelectionDialog(game_model, this).exec() == QDialog::Accepted) {
        qDebug() << "yep";
    }
}

void EntryDialog::loadMatchAction()
{

}
