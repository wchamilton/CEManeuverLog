#include "TurnHistoryCrewTab.h"
#include "ui_TurnHistoryCrewTab.h"

#include "models/GameModel.h"
#include "models/GameModelItems.h"

#include "TurnHistoryGunTab.h"

TurnHistoryCrewTab::TurnHistoryCrewTab(QSharedPointer<FilterProxy> crew_proxy, QPersistentModelIndex crew_idx, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TurnHistoryCrewTab),
    crew_proxy(crew_proxy),
    crew_idx(crew_idx)
{
    ui->setupUi(this);

    for (int i=0; i<crew_proxy->rowCount(crew_idx); ++i) {
        QPersistentModelIndex gun_idx = crew_proxy->index(i, PlaneArmamentsItem::Plane_Armaments_Name, crew_idx);
        TurnHistoryGunTab* tab = new TurnHistoryGunTab(gun_idx, ui->crew_guns_tab_widget);
        gun_tabs.insert(gun_idx.data().toString(), ui->crew_guns_tab_widget->addTab(tab, gun_idx.data().toString()));
    }
}

TurnHistoryCrewTab::~TurnHistoryCrewTab()
{
    delete ui;
}

void TurnHistoryCrewTab::updateSelectedTurnReport(const QModelIndex &crew_turn_idx)
{
    int wound_count = crew_turn_idx.siblingAtColumn(TurnCrewItem::Turn_Crew_Wounds_Accrued).data().toInt();
    ui->crew_status_lbl->setText(wound_count == 0 ? "Healthy" : wound_count == 1 ? "Lightly wounded" : wound_count == 2 ? "Severely wounded" : "Dead");
    ui->kills_scored_lbl->setText(crew_turn_idx.siblingAtColumn(TurnCrewItem::Turn_Crew_Total_Kills).data().toString());
    ui->reds_scored_lbl->setText(crew_turn_idx.siblingAtColumn(TurnCrewItem::Turn_Crew_Total_Red_Hits).data().toString());

    int action_taken_id = crew_turn_idx.siblingAtColumn(TurnCrewItem::Turn_Crew_Action_Taken).data().toInt();
    QVariant action_extra_data = crew_turn_idx.siblingAtColumn(TurnCrewItem::Turn_Crew_Action_Extra_Data).data();
    QString action_text;
    switch(action_taken_id) {
    case PlaneCrewItem::Action_None: action_text = action_extra_data.toString(); break;
    case PlaneCrewItem::Action_Shoot: {
        PlaneCrewItem::ShotProperties shot_prop = action_extra_data.value<PlaneCrewItem::ShotProperties>();
        action_text = "Shot a ";
        action_text += shot_prop.burst_len == 3 ? "long" : shot_prop.burst_len == 2 ? "medium" : "short";
        action_text += " burst at a ";
        action_text += shot_prop.target_delta == 0 ? "level" : shot_prop.target_delta > 0 ? "higher" : "lower";
        action_text += QString(" target at range %1").arg(shot_prop.target_range);
        break;
    }
    case PlaneCrewItem::Action_Reload: action_text = action_extra_data.toString(); break;
    case PlaneCrewItem::Action_Unjam: action_text = action_extra_data.toBool() ? "Successfully unjammed" : "Still jammed"; break;
    case PlaneCrewItem::Action_Drop_Payload: action_text = action_extra_data.toBool() ? "Dropped a bomb and hit" : "Dropped a bomb and missed"; break;
    case PlaneCrewItem::Action_Observe: action_text = action_extra_data.toString(); break;
    case PlaneCrewItem::Action_Custom: action_text = action_extra_data.toString(); break;
    default: break;
    }
    ui->action_taken_lbl->setText(action_text);

    for (int i=0; i<crew_turn_idx.model()->rowCount(crew_turn_idx); ++i) {
        QModelIndex gun_turn_idx = crew_turn_idx.model()->index(i, TurnArmamentItem::Turn_Crew_Armament_Idx, crew_turn_idx);
        static_cast<TurnHistoryGunTab*>(ui->crew_guns_tab_widget->widget(gun_tabs.value(gun_turn_idx.data().toModelIndex()
                                                                                              .data().toString())))->updateSelectedTurnReport(gun_turn_idx);
    }
}
