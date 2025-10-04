#include "CrewEditorTab.h"
#include "ui_CrewEditorTab.h"

#include "GunEditorTab.h"
#include "models/GameModelItems.h"
#include "models/GameModel.h"

#include <QJsonObject>
#include <QJsonArray>

CrewEditorTab::CrewEditorTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CrewEditorTab)
{
    ui->setupUi(this);
    connect(ui->add_gun_btn, &QPushButton::released, this, [&]() {
        ui->gun_tab_widget->addTab(new GunEditorTab(ui->gun_tab_widget), QString("%1's Gun").arg(ui->role_cmb->currentText()));
    });
    connect(ui->gun_tab_widget, &QTabWidget::tabCloseRequested, this, [&](int index) { ui->gun_tab_widget->removeTab(index); });
}

CrewEditorTab::~CrewEditorTab()
{
    delete ui;
}

void CrewEditorTab::populateCrewItem(FilterProxy* model, QPersistentModelIndex crew_idx)
{
    model->setData(crew_idx.sibling(crew_idx.row(), PlaneCrewItem::Plane_Crew_Role), ui->role_cmb->currentText());
    model->setData(crew_idx.sibling(crew_idx.row(), PlaneCrewItem::Plane_Crew_Can_Drop_Payloads), ui->can_drop_bombs_chk->isChecked());

    // Add gun controls
    for (int i=0; i<ui->gun_tab_widget->count(); ++i) {
        QModelIndex gun_idx = model->index(i, 0, crew_idx);
        if (!gun_idx.isValid()) {
            gun_idx = model->mapFromSource(static_cast<GameModel*>(model->sourceModel())->addGun(model->mapToSource(crew_idx)));
        }
        static_cast<GunEditorTab*>(ui->gun_tab_widget->widget(i))->populateGunItem(model, gun_idx);
    }
}

void CrewEditorTab::populateFromModel(FilterProxy* model, QPersistentModelIndex crew_idx)
{
    ui->role_cmb->setCurrentIndex(crew_idx.sibling(crew_idx.row(), PlaneCrewItem::Plane_Crew_Role_ID).data().toInt());
    ui->can_drop_bombs_chk->setChecked(crew_idx.sibling(crew_idx.row(), PlaneCrewItem::Plane_Crew_Can_Drop_Payloads).data().toBool());

    // Iterate over all the weapons and add tabs to display/modify them
    for (int i=0; i<model->rowCount(crew_idx); ++i) {
        QModelIndex gun_idx = model->index(i, 0, crew_idx);

        // Check if the child index is actually a link. If so, add its children as those are separate weapons
        if (gun_idx.data(Qt::UserRole).toInt() == BaseItem::Plane_Armaments_Link_Item_Type) {
            for (int j=0; j<model->rowCount(gun_idx); ++j) {
                QModelIndex linked_gun_idx = model->index(j, 0, gun_idx);
                GunEditorTab* tab = new GunEditorTab(ui->gun_tab_widget);
                tab->populateFromModel(linked_gun_idx);

                ui->gun_tab_widget->addTab(tab, QString("%1's %2").arg(ui->role_cmb->currentText(), linked_gun_idx.data().toString()));
            }
            continue; // In this situation, we've already handled the links so continue to the next gun/linkage
        }

        GunEditorTab* tab = new GunEditorTab(ui->gun_tab_widget);
        tab->populateFromModel(gun_idx);

        ui->gun_tab_widget->addTab(tab, QString("%1's %2").arg(ui->role_cmb->currentText(), gun_idx.data().toString()));
    }
}
