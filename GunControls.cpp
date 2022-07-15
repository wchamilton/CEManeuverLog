#include "GunControls.h"
#include "ui_GunControls.h"

GunControls::GunControls(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GunControls)
{
    ui->setupUi(this);
}

GunControls::~GunControls()
{
    delete ui;
}
