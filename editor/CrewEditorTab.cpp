#include "CrewEditorTab.h"
#include "ui_CrewEditorTab.h"

#include <QJsonArray>

#include "models/PlaneItems.h"
#include "GunEditorTab.h"

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

void CrewEditorTab::populateCrewItem(CrewItem *crew)
{
    crew->setData(CrewItem::Crew_Role, ui->role_cmb->currentText());
    crew->setData(CrewItem::Can_Drop_Bombs, ui->can_drop_bombs_chk->isChecked());
    // Add gun controls
    for (int i=0; i<ui->gun_tab_widget->count(); ++i) {
        GunItem* gun_item = new GunItem(crew);
        static_cast<GunEditorTab*>(ui->gun_tab_widget->widget(i))->populateGunItem(gun_item);
        crew->addChild(gun_item);
    }
}

void CrewEditorTab::populateFromJSON(QJsonObject crew)
{
    ui->role_cmb->setCurrentText(crew["role"].toString());
    ui->can_drop_bombs_chk->setChecked(crew["can_drop_bombs"].toBool());
    QJsonArray guns = crew["guns"].toArray();

    for (int i=0; i<guns.count(); ++i) {
        QJsonObject gun = guns.at(i).toObject();

        GunEditorTab* tab = new GunEditorTab(ui->gun_tab_widget);
        tab->populateFromJSON(gun);

        ui->gun_tab_widget->addTab(tab, QString("%1's %2").arg(crew["role"].toString()).arg(gun["name"].toString()));
    }
}
