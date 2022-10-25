#include "CrewNamesPrompt.h"
#include "ui_CrewNamesPrompt.h"
#include <QLabel>
#include <QLineEdit>
#include "models/PlaneModel.h"

CrewNamesPrompt::CrewNamesPrompt(PlaneFilterProxy *model, QPersistentModelIndex plane_idx, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CrewNamesPrompt),
    model(model),
    plane_idx(plane_idx)
{
    ui->setupUi(this);

    QGridLayout* grid = static_cast<QGridLayout*>(ui->crew_names_grid->layout());

    for (int i=0; i<model->rowCount(plane_idx); ++i) {
        grid->addWidget(new QLabel(model->index(i, CrewItem::Crew_Role, plane_idx).data().toString() + ":", this), i, 0);
        grid->addWidget(new QLineEdit(model->index(i, CrewItem::Crew_Name, plane_idx).data().toString(), this), i, 1);
    }
}

CrewNamesPrompt::~CrewNamesPrompt()
{
    delete ui;
}

void CrewNamesPrompt::accept()
{
    QGridLayout* grid = static_cast<QGridLayout*>(ui->crew_names_grid->layout());
    for (int i=0; i<model->rowCount(plane_idx); ++i) {
        model->setData(model->index(i, CrewItem::Crew_Name, plane_idx), static_cast<QLineEdit*>(grid->itemAtPosition(i, 1)->widget())->text());
    }
    QDialog::accept();
}
