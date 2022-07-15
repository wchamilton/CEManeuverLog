#include "PlanePartDamageTracker.h"
#include "ui_PlanePartDamageTracker.h"

PlanePartDamageTracker::PlanePartDamageTracker(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::PlanePartDamageTracker)
{
    ui->setupUi(this);
}

PlanePartDamageTracker::~PlanePartDamageTracker()
{
    delete ui;
}
