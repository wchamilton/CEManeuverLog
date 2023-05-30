#include "PreGamePrompt.h"
#include "ui_PreGamePrompt.h"
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include "models/PlaneModel.h"
#include "models/TurnModel.h"

PreGamePrompt::PreGamePrompt(PlaneFilterProxy *model, QPersistentModelIndex plane_idx, TurnModel *turn_model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreGamePrompt),
    plane_model(model),
    plane_idx(plane_idx),
    turn_model(turn_model)
{
    ui->setupUi(this);

    QGridLayout* grid = static_cast<QGridLayout*>(ui->crew_names_grid->layout());

    for (int i=0; i<model->rowCount(plane_idx); ++i) {
        grid->addWidget(new QLabel(model->index(i, CrewItem::Crew_Role, plane_idx).data().toString() + ":", this), i, 0);
        grid->addWidget(new QLineEdit(model->index(i, CrewItem::Crew_Name, plane_idx).data().toString(), this), i, 1);
        grid->addWidget(new QCheckBox("Ignores Deflection", this), i, 2);
        QString role_name = model->index(i, CrewItem::Crew_Role, plane_idx).data().toString();
        if (role_name == "Pilot" || role_name == "Co-Pilot") {
            grid->addWidget(new QCheckBox("Unrestricted Maneuvers", this), i, 3);
        }
    }
}

PreGamePrompt::~PreGamePrompt()
{
    delete ui;
}

void PreGamePrompt::accept()
{
    QGridLayout* grid = static_cast<QGridLayout*>(ui->crew_names_grid->layout());
    for (int i=0; i<plane_model->rowCount(plane_idx); ++i) {
        plane_model->setData(plane_model->index(i, CrewItem::Crew_Name, plane_idx), static_cast<QLineEdit*>(grid->itemAtPosition(i, 1)->widget())->text());
        plane_model->setData(plane_model->index(i, CrewItem::Has_Ignore_Deflection, plane_idx),
                             static_cast<QCheckBox*>(grid->itemAtPosition(i, 2)->widget())->isChecked());
        QString role_name = plane_model->index(i, CrewItem::Crew_Role, plane_idx).data().toString();
        if (role_name == "Pilot" || role_name == "Co-Pilot") {
            plane_model->setData(plane_model->index(i, CrewItem::Has_Unrestricted_Maneuvers, plane_idx),
                                 static_cast<QCheckBox*>(grid->itemAtPosition(i, 3)->widget())->isChecked());
        }
    }
    turn_model->setStartingValues(ui->starting_alt->value(), ui->starting_speed->value());
    QDialog::accept();
}
