#include "PlanePartDamageTracker.h"
#include "ui_PlanePartDamageTracker.h"

PlanePartDamageTracker::PlanePartDamageTracker(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::PlanePartDamageTracker)
{
    ui->setupUi(this);
    connect(ui->damage_btn, &QPushButton::pressed, this, &PlanePartDamageTracker::takeDamage);
    connect(ui->current_hp, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &PlanePartDamageTracker::setBorderColour);
}

PlanePartDamageTracker::~PlanePartDamageTracker()
{
    delete ui;
}

void PlanePartDamageTracker::setModelIndexes(QPersistentModelIndex hp, QPersistentModelIndex critical_hp)
{
    this->critical_hp = critical_hp.data().toInt();

    ui->current_hp->setValue(hp.data().toInt());
    ui->current_hp->setMaximum(hp.data().toInt());
    ui->current_hp->setSuffix(QString("/%1").arg(hp.data().toInt()));
    setEnabled(true);
    setStyleSheet("border-color:green");
}

void PlanePartDamageTracker::clear()
{
    ui->current_hp->setValue(0);
    ui->current_hp->setSuffix("");
    setEnabled(false);
    setStyleSheet("border-color:silver");
}

void PlanePartDamageTracker::takeDamage()
{
    ui->current_hp->setValue(ui->current_hp->value()-1);
}

void PlanePartDamageTracker::setBorderColour(int value)
{
    if (value == 0) {
        setStyleSheet("border-color:gray");
        ui->status_lbl->setText(tr("Destroyed"));
        setEnabled(false);
    }
    else if (value <= critical_hp) {
        setStyleSheet("border-color:red");
        ui->status_lbl->setText(tr("Critical"));
    }
    else {
        setStyleSheet("border-color:green");
        ui->status_lbl->setText(tr("Normal"));
    }
}
