#include "TurnHistoryDialog.h"
#include "ui_TurnHistoryDialog.h"

#include "models/GameModel.h"
#include "models/GameModelItems.h"

#include "TurnHistoryCrewTab.h"

TurnHistoryDialog::TurnHistoryDialog(GameModel *game_model, QSharedPointer<FilterProxy> crew_proxy, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TurnHistoryDialog),
    game_model(game_model)
{
    ui->setupUi(this);
    ui->turnListView->setModel(game_model);
    ui->turnListView->setRootIndex(game_model->turnsRootIdx());
    ui->turnListView->setModelColumn(TurnItem::Turn_Number);
    ui->matchNameLbl->setText(game_model->gameRootIdx().siblingAtColumn(GameItem::Game_Conflict_Name).data().toString());
    QModelIndex plane_idx = game_model->gameRootIdx().siblingAtColumn(GameItem::Game_Plane_Selected).data().toModelIndex();
    ui->planeTurnHistoryGrpBox->setTitle(plane_idx.siblingAtColumn(PlaneItem::Plane_Name).data().toString() + " Turn 0");

    for (int i=0; i<crew_proxy->rowCount(crew_proxy->mapFromSource(plane_idx)); ++i) {
        QPersistentModelIndex crew_idx = crew_proxy->index(i, PlaneCrewItem::Plane_Crew_Name, crew_proxy->mapFromSource(plane_idx));
        TurnHistoryCrewTab* tab = new TurnHistoryCrewTab(crew_proxy, crew_idx);
        crew_tabs.insert(crew_idx.data().toString(), ui->crewTurnHistoryTabWidget->addTab(tab, crew_idx.data().toString()));
    }

    connect(ui->turnListView->selectionModel(), &QItemSelectionModel::currentChanged, [=](const QModelIndex &current, const QModelIndex&){
        updateSelectedTurnReport(current);
    });
}

TurnHistoryDialog::~TurnHistoryDialog()
{
    delete ui;
}

void TurnHistoryDialog::showEvent(QShowEvent *event)
{
    updateSelectedTurnReport(game_model->currentTurn(TurnItem::Turn_Number));
    QDialog::showEvent(event);
}

void TurnHistoryDialog::updateSelectedTurnReport(const QModelIndex &selected_turn)
{
    QModelIndex plane_idx = game_model->gameRootIdx().siblingAtColumn(GameItem::Game_Plane_Selected).data().toModelIndex();

    int prev_alt = selected_turn.siblingAtColumn(TurnItem::Turn_Plane_Alt_Last_Turn).data().toInt();
    int current_alt = selected_turn.siblingAtColumn(TurnItem::Turn_Plane_Alt).data().toInt();

    int prev_speed = selected_turn.siblingAtColumn(TurnItem::Turn_Plane_Speed_Last_Turn).data().toInt();
    int current_speed = selected_turn.siblingAtColumn(TurnItem::Turn_Selected_Maneuver_Idx).data().toModelIndex().
                        siblingAtColumn(PlaneManeuverItem::Plane_Maneuver_Speed).data().toInt();

    int remaining_fuel = selected_turn.siblingAtColumn(TurnItem::Turn_Plane_Fuel).data().toInt();
    int total_fuel = game_model->gameRootIdx().siblingAtColumn(GameItem::Game_Plane_Selected).data().toModelIndex()
                         .siblingAtColumn(PlaneItem::Plane_Fuel_Cap).data().toInt();

    // Update labels and group box title
    ui->planeTurnHistoryGrpBox->setTitle(plane_idx.siblingAtColumn(PlaneItem::Plane_Name).data().toString() + " - " + selected_turn.data().toString());
    ui->selectedManeuverLbl->setText(selected_turn.siblingAtColumn(TurnItem::Turn_Selected_Maneuver_Idx).data().toModelIndex()
                                         .siblingAtColumn(PlaneManeuverItem::Plane_Maneuver_Name).data().toString());
    ui->altChangeLbl->setText(QString("%1 -> %2").arg(prev_alt).arg(current_alt));
    ui->speedChangeLbl->setText(QString("%1 -> %2").arg(prev_speed).arg(current_speed));
    ui->fuelUsedLbl->setText(QString("%1").arg(selected_turn.siblingAtColumn(TurnItem::Turn_Plane_Fuel_Used).data().toInt()));
    ui->fuelRemainingLbl->setText(QString("%1/%2").arg(remaining_fuel).arg(total_fuel));

    // Iterate over the turn children. This comprises of crew and active effects
    ui->active_effects_list->clear();
    for (int i=0; i<game_model->rowCount(selected_turn); ++i) {
        QModelIndex turn_child_idx = game_model->index(i, 0, selected_turn);
        // Aggregate active effecs to be displayed
        if (turn_child_idx.data(Qt::UserRole).toInt() == BaseItem::Turn_Effects_Type) {
            QString active_effect_text = turn_child_idx.siblingAtColumn(TurnPlaneEffects::Turn_Plane_Effect_Name).data().toString();
            int remaining_turns = turn_child_idx.siblingAtColumn(TurnPlaneEffects::Turn_Plane_Effect_Remaining_Turns).data().toInt();
            if (remaining_turns > 0) {
                active_effect_text += QString("\n (%1 turn%2 remain)").arg(remaining_turns).arg(remaining_turns == 1 ? "" : "s");
            }
            if (active_effect_text.isEmpty()) {
                active_effect_text = "No effects currently applied";
            }
            QListWidgetItem* item = new QListWidgetItem(active_effect_text, ui->active_effects_list);
            item->setToolTip(turn_child_idx.siblingAtColumn(TurnPlaneEffects::Turn_Plane_Effect_Desc).data().toString().arg(remaining_turns));
        }
        // Trigger updates for crew
        else if (turn_child_idx.data(Qt::UserRole).toInt() == BaseItem::Turn_Crew_Item_Type) {
            static_cast<TurnHistoryCrewTab*>(ui->crewTurnHistoryTabWidget->widget(crew_tabs.value(turn_child_idx.data().toModelIndex()
                                                                                                   .data().toString())))->updateSelectedTurnReport(turn_child_idx);
        }
    }
}
