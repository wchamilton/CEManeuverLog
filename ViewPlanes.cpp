#include "ViewPlanes.h"
#include "ui_ViewPlanes.h"

ViewPlanes::ViewPlanes(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewPlanes)
{
    ui->setupUi(this);
}

ViewPlanes::~ViewPlanes()
{
    delete ui;
}
