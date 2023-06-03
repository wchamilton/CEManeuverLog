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

    for (int row=0, crew_idx=0; crew_idx<model->rowCount(plane_idx); ++row, ++crew_idx) {
        CrewInputWidgets widgets;

        // Prepare the crew name input
        QString role_name = model->index(crew_idx, CrewItem::Crew_Role, plane_idx).data().toString();
        QPersistentModelIndex crew_name_idx = model->index(crew_idx, CrewItem::Crew_Name, plane_idx);
        widgets.crew_name_input = new QLineEdit(this);
        grid->addWidget(new QLabel(role_name + ":", this), row, 0, (role_name == "Pilot" || role_name == "Co-Pilot" ? 2 : 1), 1);
        grid->addWidget(widgets.crew_name_input, row, 1, (role_name == "Pilot" || role_name == "Co-Pilot" ? 2 : 1), 1);

        // Prepare the control for toggling if the crew member can ignore deflection
        QPersistentModelIndex crew_ignores_deflection_idx = model->index(crew_idx, CrewItem::Has_Ignore_Deflection, plane_idx);
        widgets.ignores_deflection = new QCheckBox("Ignores Deflection", this);
        grid->addWidget(widgets.ignores_deflection, row, 2);

        // If the crew member is a pilot, add a control to toggle if they have the unrestricted maneuvers ability
        if (role_name == "Pilot" || role_name == "Co-Pilot") {
            QPersistentModelIndex crew_unrestricted_maneuvers_idx = model->index(crew_idx, CrewItem::Has_Unrestricted_Maneuvers, plane_idx);
            widgets.unrestricted_maneuvers = new QCheckBox("Unrestricted Maneuvers", this);
            grid->addWidget(widgets.unrestricted_maneuvers, ++row, 2);
        }

        // Add the input collection, paired with the crew member's index, to the list for later access
        inputs_by_index.append({crew_name_idx, widgets});

        // Add lines for spacing
        if (crew_idx < model->rowCount(plane_idx)-1) {
            QFrame* line = new QFrame(this);
            line->setFrameShape(QFrame::HLine);
            line->setFrameShadow(QFrame::Sunken);
            grid->addWidget(line, ++row, 0, 1, 3);
        }
    }
}

PreGamePrompt::~PreGamePrompt()
{
    delete ui;
}

void PreGamePrompt::accept()
{
    for (auto input : inputs_by_index) {
        QPersistentModelIndex crew_name_idx = input.first;
        plane_model->setData(crew_name_idx, input.second.crew_name_input->text());
        plane_model->setData(crew_name_idx.sibling(crew_name_idx.row(), CrewItem::Has_Ignore_Deflection), input.second.ignores_deflection->isChecked());
        plane_model->setData(crew_name_idx.sibling(crew_name_idx.row(), CrewItem::Has_Unrestricted_Maneuvers),
                             input.second.unrestricted_maneuvers ? input.second.unrestricted_maneuvers->isChecked() : false);
    }
    plane_model->setData(plane_idx.sibling(plane_idx.row(), PlaneItem::Bombs_Carried), ui->has_bombs->isChecked() ? ui->bomb_count->value() : 0);
    turn_model->setStartingValues(ui->starting_alt->value(), ui->starting_speed->value());
    QDialog::accept();
}
