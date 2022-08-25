#include "CrewTurnOptions.h"
#include "ui_CrewTurnOptions.h"

CrewTurnOptions::CrewTurnOptions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CrewTurnOptions)
{
    ui->setupUi(this);
    connect(ui->actionSelection, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), ui->actionStack, &QStackedWidget::setCurrentIndex);
}

void CrewTurnOptions::addAction(QString actionName, QWidget* actionWidget)
{
    ui->actionSelection->addItem(actionName); ui->actionStack->addWidget(actionWidget);
}

CrewTurnOptions::~CrewTurnOptions()
{
    delete ui;
}
