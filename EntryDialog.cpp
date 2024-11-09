#include "EntryDialog.h"
#include "ui_EntryDialog.h"
#include "CEManeuvers.h"
#include "editor/PlaneEditor.h"

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
    setColour(ui->newPlaneBtn, QColor("#4ab589"));
    setColour(ui->editPlaneBtn, QColor("#B54A76"));
    setColour(ui->newMatchBtn, QColor("#D1A72E"));
    setColour(ui->loadMatchBtn, QColor("#2E58D1"));

    connect(ui->newPlaneBtn, &QPushButton::clicked, this, [=](){
        PlaneEditor(this).exec();
    });
    connect(ui->exitBtn, &QPushButton::clicked, this, [=](){
        QApplication::quit();
    });

    connect(ui->editPlaneBtn, &QPushButton::clicked, this, &EntryDialog::editPlaneAction);
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
        QFile file(fileDlg.selectedFiles().first());
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

}

void EntryDialog::loadMatchAction()
{

}
