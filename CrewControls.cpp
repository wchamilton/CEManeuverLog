#include "CrewControls.h"
#include "ui_CrewControls.h"

CrewControls::CrewControls(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CrewControls)
{
    ui->setupUi(this);
}

CrewControls::~CrewControls()
{
    delete ui;
}
