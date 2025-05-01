#include "EntryDialog.h"
#include "ui_EntryDialog.h"

#include "models/GameModel.h"
#include "editor/PlaneEditor.h"
#include "editor/PlaneEditorSelector.h"
#include "turn_logger/PlaneSelectionDialog.h"
#include "turn_logger/TurnTrackerDialog.h"

#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QTreeView>

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
    GameModel game_model(this);
    PlaneEditorSelector selector(this);
    selector.setModel(&game_model, game_model.planesRootIdx());
    if (selector.exec()) {
        PlaneEditor(&game_model, selector.getSelectedPlane()).exec();
    }
}

void EntryDialog::newMatchAction()
{
    GameModel game_model;
    PlaneSelectionDialog selector(&game_model, this);
    if (selector.exec() == QDialog::Accepted) {
        TurnTrackerDialog(&game_model, this).exec();
    }
}

void EntryDialog::loadMatchAction()
{

}
