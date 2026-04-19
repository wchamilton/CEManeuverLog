#include "PlaneEditor.h"
#include "ui_PlaneEditor.h"

#include "CEManeuvers.h"
#include "CrewEditorTab.h"
#include "models/GameModel.h"
#include "models/GameModelItems.h"
#include "graphics/ManeuverScene.h"

#include <QStandardItem>
#include <QMenu>
#include <QToolButton>
#include <QInputDialog>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>

PlaneEditor::PlaneEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlaneEditor)
{
    ui->setupUi(this);

    // Initialize a blank plane
    init();
}

PlaneEditor::PlaneEditor(GameModel* game_model, QPersistentModelIndex plane_idx, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlaneEditor),
    game_model(game_model),
    plane_idx(plane_idx)
{
    ui->setupUi(this);

    // Populate the maneuvers from the given plane
    init();

    // Populate the rest of the fields with data from the plane
    ui->plane_name->setText(plane_idx.data().toString());
    if (plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Era).data().toInt() == PlaneItem::Era_Late_War) {
        ui->late_war_btn->setChecked(true);
    }
    else {
        ui->early_war_btn->setChecked(true);
    }

    QPersistentModelIndex filtered_plane_idx = crew_proxy->mapFromSource(plane_idx);

    // Populate the pilot. Use widget at index 1 because index 0 is used by the '+' tab
    QPersistentModelIndex pilot_idx = crew_proxy->index(0, 0, filtered_plane_idx);
    static_cast<CrewEditorTab*>(ui->crew_editor_tab->widget(1))->populateFromModel(crew_proxy, pilot_idx);

    // Populate any crew after the pilot
    for (int i=1; i<crew_proxy->rowCount(filtered_plane_idx); ++i) {
        CrewEditorTab* tab = new CrewEditorTab(ui->crew_editor_tab);
        tab->populateFromModel(crew_proxy, crew_proxy->index(i, 0, filtered_plane_idx));
        ui->crew_editor_tab->addTab(tab, QString("Crew %1").arg(i));
    }

    // Populate the plane's attributes
    ui->fuel_amt->setValue(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Fuel_Cap).data().toInt());
    ui->engine_hp->setValue(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Engine_HP).data().toInt());
    ui->engine_critical_hp->setValue(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Engine_Critical).data().toInt());
    ui->wing_hp->setValue(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Wing_HP).data().toInt());
    ui->wing_critical_hp->setValue(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Wing_Critical).data().toInt());
    ui->fuselage_hp->setValue(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Fuselage_HP).data().toInt());
    ui->fuselage_critical_hp->setValue(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Fuselage_Critical).data().toInt());
    ui->tail_hp->setValue(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Tail_HP).data().toInt());
    ui->tail_critical_hp->setValue(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Tail_Critical).data().toInt());
    ui->rated_climb->setValue(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Rated_Climb).data().toInt());
    ui->rated_dive->setValue(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Rated_Dive).data().toInt());
    ui->max_alt->setText(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Max_Altitude).data().toString());
    ui->stab_rating->setText(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Stability_Rating).data().toString());
}

PlaneEditor::~PlaneEditor()
{
    delete ui;
}

void PlaneEditor::handleManeuverCheck(QListWidgetItem* item)
{
    Maneuver m = item->data(Qt::UserRole).value<Maneuver>();
    if (item->checkState() == Qt::Checked) {
        maneuver_idx_map[m.name] = game_model->addManeuver(m, plane_idx);
        maneuver_scene->addManeuver(maneuver_idx_map[m.name]);
    }
    else {
        maneuver_scene->removeManeuver(maneuver_idx_map[m.name]);
        game_model->removeManeuver(maneuver_idx_map.take(m.name));
    }
}

void PlaneEditor::handleManeuverListContextMenu(const QPoint &pos)
{
    // Handle global position
    QPoint globalPos = ui->maneuver_list->mapToGlobal(pos);

    // Create menu with checkstate controls
    QMenu menu;
    menu.addAction("Add selected maneuvers", this, [=](){
        for (auto item : ui->maneuver_list->selectedItems()) {
            item->setCheckState(Qt::Checked);
        }
    });
    menu.addAction("Remove selected maneuvers", this, [=](){
        for (auto item : ui->maneuver_list->selectedItems()) {
            item->setCheckState(Qt::Unchecked);
        }
    });
    menu.exec(globalPos);
}

void PlaneEditor::handleManeuverPropertyChanges(const QVariant &arg)
{
    for (auto item : ui->maneuver_list->selectedItems()) {
        Maneuver m = item->data(Qt::UserRole).value<Maneuver>();
        game_model->setData(maneuver_idx_map[m.name].sibling(maneuver_idx_map[m.name].row(), sender()->property("col").toInt()), arg);
    }
    maneuver_scene->update();
}

void PlaneEditor::exportJSON()
{
    game_model->setData(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Name), ui->plane_name->text().simplified());
    game_model->setData(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Era), ui->early_war_btn->isChecked() ? PlaneItem::Era_Early_War : PlaneItem::Era_Late_War);
    game_model->setData(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Fuel_Cap), ui->fuel_amt->value());
    game_model->setData(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Engine_HP), ui->engine_hp->value());
    game_model->setData(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Engine_Critical), ui->engine_critical_hp->value());
    game_model->setData(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Wing_HP), ui->wing_hp->value());
    game_model->setData(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Wing_Critical), ui->wing_critical_hp->value());
    game_model->setData(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Fuselage_HP), ui->fuselage_hp->value());
    game_model->setData(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Fuselage_Critical), ui->fuselage_critical_hp->value());
    game_model->setData(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Tail_HP), ui->tail_hp->value());
    game_model->setData(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Tail_Critical), ui->tail_critical_hp->value());
    game_model->setData(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Rated_Climb), ui->rated_climb->value());
    game_model->setData(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Rated_Dive), ui->rated_dive->value());
    game_model->setData(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Max_Altitude), ui->max_alt->text().simplified());
    game_model->setData(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Stability_Rating), ui->stab_rating->text().simplified());

    QModelIndex filtered_plane_idx = crew_proxy->mapFromSource(plane_idx);

    // Start at 1 since index 0 is used by the [+] tab
    for (int i=1; i<ui->crew_editor_tab->count(); ++i) {
        QModelIndex crew_idx = crew_proxy->index(i-1, 0, filtered_plane_idx);
        static_cast<CrewEditorTab*>(ui->crew_editor_tab->widget(i))->populateCrewItem(crew_proxy, crew_idx);
    }

    QString file_path = QFileDialog::getSaveFileName(this, tr("Save Plane"), PLANES_LOCATION + QDir::separator() + ui->plane_name->text(), tr("JSON files (*.json)"));
    if (file_path != "") {
        QFileInfo file_info(file_path);
        if (file_info.suffix() != "json") {
            file_info.setFile(file_path + ".json");
        }

        QFile file(file_info.absoluteFilePath());
        if (!file.open(QIODevice::WriteOnly|QIODevice::Text)) {
            qWarning() << "Could not open" << file.fileName();
            return;
        }

        QTextStream out(&file);
        out.setAutoDetectUnicode(true);
        BaseItem* plane_item_ptr = static_cast<BaseItem*>(plane_idx.internalPointer());
        out << QJsonDocument(plane_item_ptr->toJSON()).toJson();
        out.flush();
        file.close();
    }

    // Clean up crew to avoid creating duplicate/extra entries
    for (int i=0; i<crew_proxy->rowCount(filtered_plane_idx); ++i) {
        game_model->removeChild(i, crew_proxy->mapToSource(filtered_plane_idx));
    }
}

void PlaneEditor::init()
{
    // If we're creating a new plane, need to create a new instance of the game model
    if (game_model == nullptr) {
        game_model = new GameModel(this);
    }

    // Initialize a couple proxy models to isolate crew and maneuvers
    crew_proxy = new FilterProxy(game_model, this); // Init a proxy to filter for crew
    crew_proxy->setTypeFilter({BaseItem::Plane_Item_Type, BaseItem::Plane_Crew_Item_Type, BaseItem::Plane_Armaments_Link_Item_Type, BaseItem::Plane_Armaments_Item_Type});
    maneuver_proxy = new FilterProxy(game_model, this); // Init a proxy to filter for maneuvers
    maneuver_proxy->setTypeFilter({BaseItem::Plane_Item_Type, BaseItem::Plane_Maneuver_Item_Type});

    // If we're creating a new plane, add it to the model and return the index
    if (!plane_idx.isValid()) {
        plane_idx = game_model->addPlane(); // Create empty plane to populate
    }

    // Convert the plane index from the base model to the maneuver proxy model and set that to the scene
    QPersistentModelIndex filtered_plane_idx = maneuver_proxy->mapFromSource(plane_idx);
    maneuver_scene = new ManeuverScene(maneuver_proxy, filtered_plane_idx, ui->maneuver_schedule_gv);
    ui->maneuver_schedule_gv->setScene(maneuver_scene);

    // Init controls to add more crew
    QToolButton *tb = new QToolButton(ui->crew_editor_tab);
    tb->setAutoRaise(true);
    tb->setText("+");

    // Add empty, not enabled tab to tabWidget
    ui->crew_editor_tab->addTab(new QLabel("Add tabs by pressing \"+\""), QString());
    ui->crew_editor_tab->setTabEnabled(0, false);

    // Add tab button to current tab. Button will be enabled, but tab -- not
    ui->crew_editor_tab->tabBar()->setTabButton(0, QTabBar::RightSide, tb);

    // Manually add the spin maneuver to the schedule
    Maneuver spin_man = master_maneuver_map["0S1"];
    spin_man.tolerances = "-/-/D1";
    maneuver_idx_map[spin_man.name] = maneuver_proxy->mapFromSource(game_model->addManeuver(spin_man, plane_idx));
    maneuver_scene->addManeuver(maneuver_idx_map[spin_man.name]);

    // Add initial crew member to the tab and set the index accordingly
    int pilot_index = ui->crew_editor_tab->addTab(new CrewEditorTab(ui->crew_editor_tab), "Pilot");
    ui->crew_editor_tab->setCurrentIndex(pilot_index);
    ui->crew_editor_tab->tabBar()->tabButton(pilot_index, QTabBar::RightSide)->resize(0, 0);

    // Fetch the maneuvers from the master list so that they can be added to the list widget, sorted in the desired way (speed followed by restricted)
    QList<Maneuver> maneuvers = master_maneuver_map.values();
    std::sort(maneuvers.begin(), maneuvers.end()); // Convert the map to a list so that we can sort the maneuvers by speed

    // Iterate over the maneuvers, adding all to the list widget and any existing to the scene if a plane was loaded
    for (const Maneuver &m : maneuvers) {
        if (m.name == "0S1") {
            continue; // No point in adding 0S1 (spin) as an option; it's always mandatory to have
        }

        // Initialize the list widget entry
        QListWidgetItem* item = new QListWidgetItem(m.name);

        // In the case of editing a plane, we should set the checkstate correctly for maneuvers already present
        bool already_added = false;
        for (int i=0; i<maneuver_proxy->rowCount(filtered_plane_idx); ++i) {
            QPersistentModelIndex filtered_maneuver_idx = maneuver_proxy->index(i, PlaneManeuverItem::Plane_Maneuver_Name, filtered_plane_idx);
            if (m.name == filtered_maneuver_idx.data().toString()) {
                already_added = true;
                maneuver_idx_map[m.name] = maneuver_proxy->mapToSource(filtered_maneuver_idx);
                break;
            }
        }

        // Apply all data as configured above (checkstate and maneuver struct)
        item->setCheckState(already_added ? Qt::Checked : Qt::Unchecked);
        item->setData(Qt::UserRole, QVariant::fromValue(m));
        ui->maneuver_list->addItem(item);
    }

    ui->set_climb->setProperty("col", PlaneManeuverItem::Plane_Maneuver_Climb_Val);
    ui->set_level->setProperty("col", PlaneManeuverItem::Plane_Maneuver_Level_Val);
    ui->set_dive->setProperty("col", PlaneManeuverItem::Plane_Maneuver_Dive_Val);
    ui->set_can_reload->setProperty("col", PlaneManeuverItem::Plane_Maneuver_Can_Reload);
    ui->set_obs_can_reload->setProperty("col", PlaneManeuverItem::Plane_Maneuver_Observer_Can_Reload);
    ui->set_put_out_fires_bonus->setProperty("col", PlaneManeuverItem::Plane_Maneuver_Put_Out_Fires_Bonus);
    ui->set_can_repeat->setProperty("col", PlaneManeuverItem::Plane_Maneuver_Can_Be_Repeated);
    ui->set_weight_restricted->setProperty("col", PlaneManeuverItem::Plane_Maneuver_Has_Weight_Restriction);

    connect(ui->maneuver_list, &QListWidget::itemChanged, this, &PlaneEditor::handleManeuverCheck);
    connect(ui->maneuver_list, &QListWidget::customContextMenuRequested, this, &PlaneEditor::handleManeuverListContextMenu);
    connect(ui->crew_editor_tab, &QTabWidget::tabCloseRequested, this, [&](int index) { ui->crew_editor_tab->removeTab(index); });
    connect(tb, &QToolButton::clicked, this, [&]() {
        int crew_count = ui->crew_editor_tab->tabBar()->count()-1; // subtract one for the button
        if (crew_count < 5) {
            ui->crew_editor_tab->addTab(new CrewEditorTab(ui->crew_editor_tab), QString("Crew %1").arg(crew_count));
        }
    });
    connect(ui->crew_editor_tab->tabBar(), &QTabBar::tabBarDoubleClicked, this, [&](int index) {
        // ignore clicks on the [+] tab
        if (index == 0) {
            return;
        }
        QString new_text = QInputDialog::getText(this, "New Tab Name", "Tab name:", QLineEdit::Normal, ui->crew_editor_tab->tabText(index));
        if (!new_text.isNull()) {
            ui->crew_editor_tab->setTabText(index, new_text);
        }
    });
    connect(ui->set_climb, &QComboBox::currentTextChanged, this, &PlaneEditor::handleManeuverPropertyChanges);
    connect(ui->set_level, &QComboBox::currentTextChanged, this, &PlaneEditor::handleManeuverPropertyChanges);
    connect(ui->set_dive, &QComboBox::currentTextChanged, this, &PlaneEditor::handleManeuverPropertyChanges);
    connect(ui->set_can_reload, &QCheckBox::checkStateChanged, this, &PlaneEditor::handleManeuverPropertyChanges);
    connect(ui->set_obs_can_reload, &QCheckBox::checkStateChanged, this, &PlaneEditor::handleManeuverPropertyChanges);
    connect(ui->set_put_out_fires_bonus, &QCheckBox::checkStateChanged, this, &PlaneEditor::handleManeuverPropertyChanges);
    connect(ui->set_can_repeat, &QCheckBox::checkStateChanged, this, &PlaneEditor::handleManeuverPropertyChanges);
    connect(ui->set_weight_restricted, &QCheckBox::checkStateChanged, this, &PlaneEditor::handleManeuverPropertyChanges);
    connect(ui->export_json, &QPushButton::pressed, this, &PlaneEditor::exportJSON);
}
