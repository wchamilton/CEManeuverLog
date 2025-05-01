#include "PlaneSelectionDialog.h"
#include "ui_PlaneSelectionDialog.h"

#include "models/GameModel.h"
#include "models/GameModelItems.h"
#include "graphics/ManeuverScene.h"

#include <QGraphicsScene>
#include <QCheckBox>

PlaneSelectionDialog::PlaneSelectionDialog(GameModel *game_model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlaneSelectionDialog),
    game_model(game_model)
{
    ui->setupUi(this);

    // Populate tree widget with planes
    QModelIndex planes_root = game_model->planesRootIdx();
    for (int i=0; i<game_model->rowCount(planes_root); ++i) {
        QPersistentModelIndex plane_idx = game_model->index(i, PlaneItem::Plane_Name, planes_root);
        QTreeWidgetItem* plane_tree_item = new QTreeWidgetItem();
        plane_tree_item->setData(0, Qt::DisplayRole, plane_idx.data().toString());
        plane_tree_item->setData(0, Qt::UserRole, plane_idx);

        switch(game_model->index(plane_idx.row(), PlaneItem::Plane_Era, planes_root).data().toInt()) {
            case PlaneItem::Era_Early_War: ui->planes_tree->topLevelItem(0)->addChild(plane_tree_item); break;
            case PlaneItem::Era_Late_War:  ui->planes_tree->topLevelItem(1)->addChild(plane_tree_item); break;
            default: qWarning() << QString("Plane %1 was loaded with unknown era").arg(plane_idx.data().toString());
        }
    }
    ui->planes_tree->expandAll();
    ui->planes_tree->resizeColumnToContents(0);

    // Initialize proxy models
    maneuver_proxy = QSharedPointer<FilterProxy>::create(game_model, this);
    maneuver_proxy->setTypeFilter(BaseItem::Plane_Maneuver_Item_Type);
    maneuver_proxy->setRecursiveFilteringEnabled(true);

    crew_proxy = QSharedPointer<FilterProxy>::create(game_model, this);
    crew_proxy->setTypeFilter(BaseItem::Plane_Crew_Item_Type);
    crew_proxy->setRecursiveFilteringEnabled(true);
    crew_proxy->setAutoAcceptChildRows(true);

    // We don't want to enable maneuver selection in this dialog
    ui->maneuver_sched->setInteractive(false);

    connect(ui->planes_tree, &QTreeWidget::currentItemChanged, this, &PlaneSelectionDialog::planeSelected);
    connect(ui->player_name, &QLineEdit::textEdited, this, &PlaneSelectionDialog::checkIfStartReady);
    connect(ui->combat_name, &QLineEdit::textEdited, this, &PlaneSelectionDialog::checkIfStartReady);
}

PlaneSelectionDialog::~PlaneSelectionDialog()
{
    delete ui;
}

void PlaneSelectionDialog::accept()
{
    QPersistentModelIndex plane_idx = ui->planes_tree->currentItem()->data(0, Qt::UserRole).toPersistentModelIndex();
    QModelIndex game_idx = game_model->gameRootIdx();
    game_model->setData(game_idx.sibling(game_idx.row(), GameItem::Game_Plane_Selected), plane_idx);
    game_model->setData(game_idx.sibling(game_idx.row(), GameItem::Game_Player), ui->player_name->text());
    game_model->setData(game_idx.sibling(game_idx.row(), GameItem::Game_Conflict_Name), ui->combat_name->text());
    game_model->setData(game_idx.sibling(game_idx.row(), GameItem::Game_Conflict_Date), ui->combat_date->date());
    game_model->setData(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Current_Speed), ui->starting_speed_spn->value());
    game_model->setData(plane_idx.sibling(plane_idx.row(), PlaneItem::Plane_Current_Alt), ui->starting_alt_spn->value());

    QDialog::accept();
}

void PlaneSelectionDialog::planeSelected(QTreeWidgetItem *current, QTreeWidgetItem *prev)
{
    if (current == ui->planes_tree->topLevelItem(0) || current == ui->planes_tree->topLevelItem(1)) {
        ui->start_button->setEnabled(false);
        return;
    }
    if (current == prev) {
        return;
    }
    QPersistentModelIndex plane_idx = current->data(0, Qt::UserRole).toPersistentModelIndex();

    // Set up the plane's maneuver schedule
    if (!scene_map.contains(plane_idx.data().toString())) {
        scene_map[plane_idx.data().toString()] = new ManeuverScene(maneuver_proxy.data(), maneuver_proxy->mapFromSource(plane_idx), this);
    }
    ui->maneuver_sched->setScene(scene_map[plane_idx.data().toString()]);

    auto plane_data = [plane_idx] (int col) {
        return plane_idx.sibling(plane_idx.row(), col).data();
    };

    ui->starting_alt_spn->setMaximum(plane_data(PlaneItem::Plane_Max_Altitude).toInt());

    // Dump the plane metadata
    ui->plane_grpbox->setTitle(plane_idx.data().toString());
    ui->fuel_val->setText(plane_data(PlaneItem::Plane_Fuel_Cap).toString());
    ui->engine_hp_val->setText(plane_data(PlaneItem::Plane_Engine_HP).toString());
    ui->engine_critical_hp_val->setText(plane_data(PlaneItem::Plane_Engine_Critical).toString());
    ui->wing_hp_val->setText(plane_data(PlaneItem::Plane_Wing_HP).toString());
    ui->wing_critical_hp_val->setText(plane_data(PlaneItem::Plane_Wing_Critical).toString());
    ui->fuse_hp_val->setText(plane_data(PlaneItem::Plane_Fuselage_HP).toString());
    ui->fuse_critical_hp_val->setText(plane_data(PlaneItem::Plane_Fuselage_Critical).toString());
    ui->tail_hp_val->setText(plane_data(PlaneItem::Plane_Tail_HP).toString());
    ui->tail_critical_hp_val->setText(plane_data(PlaneItem::Plane_Tail_Critical).toString());
    ui->rated_climb_val->setText(plane_data(PlaneItem::Plane_Rated_Climb).toString());
    ui->rated_dive_val->setText(plane_data(PlaneItem::Plane_Rated_Dive).toString());
    ui->max_alt_val->setText(plane_data(PlaneItem::Plane_Max_Altitude).toString() +
                             (plane_data(PlaneItem::Plane_Can_Return_To_Max_Alt).toBool() ? "+" : ""));
    ui->stability_val->setText(plane_data(PlaneItem::Plane_Stability_Rating).toString());

    // Populate new crew widgets if needed before switching to the stacked widget
    if (!crew_widget_map.contains(plane_idx.row())) {
        QWidget* crew_page_widget = new QWidget(ui->crew_stacked_widget);
        QGridLayout* grid = new QGridLayout(crew_page_widget);
        crew_page_widget->setLayout(grid);
        QModelIndex filtered_plane_idx = crew_proxy->mapFromSource(plane_idx);
        for (int i=0; i<crew_proxy->rowCount(filtered_plane_idx); ++i) {
            prepareCrewRows(grid, crew_proxy->index(i, PlaneCrewItem::Plane_Crew_Name, filtered_plane_idx));
            if (i < crew_proxy->rowCount(filtered_plane_idx)-1) {
                QFrame* line = new QFrame(grid->parentWidget());
                line->setFrameShape(QFrame::HLine);
                line->setFrameShadow(QFrame::Sunken);
                grid->addWidget(line, grid->rowCount(), 0, 1, 2);
            }
        }
        grid->setRowStretch(grid->rowCount(), 1);
        crew_widget_map[plane_idx.row()] = crew_page_widget;
        ui->crew_stacked_widget->insertWidget(plane_idx.row(), crew_page_widget);
    }
    ui->crew_stacked_widget->setCurrentWidget(crew_widget_map[plane_idx.row()]);
}

void PlaneSelectionDialog::checkIfStartReady()
{
    ui->start_button->setDisabled(ui->player_name->text().isEmpty() || ui->combat_name->text().isEmpty());
}

void PlaneSelectionDialog::prepareCrewRows(QGridLayout* layout, QPersistentModelIndex crew_idx)
{
    int crew_role_id = crew_idx.sibling(crew_idx.row(), PlaneCrewItem::Plane_Crew_Role_ID).data().toInt();
    QString crew_role = crew_idx.sibling(crew_idx.row(), PlaneCrewItem::Plane_Crew_Role).data().toString();
    int row = layout->rowCount();
    layout->addWidget(new QLabel(crew_role, layout->parentWidget()), row, 0, 1, 1);

    // Crew name input handling
    QLineEdit* crew_name_input = new QLineEdit(layout->parentWidget());
    layout->addWidget(crew_name_input, row++, 1, 1, 2);
    connect(crew_name_input, &QLineEdit::textEdited, this, [crew_idx, this](QString crew_name){
        crew_proxy->setData(crew_idx, crew_name);
    });

    // Ability 'Ignores Deflection' handling. Only create if crew has a gun
    if (crew_proxy->rowCount(crew_idx)) {
        QCheckBox* has_ignore_deflection = new QCheckBox("Ignores Deflection", layout->parentWidget());
        layout->addWidget(has_ignore_deflection, row++, 1);
        connect(has_ignore_deflection, &QCheckBox::clicked, this, [crew_idx, this](bool arg){
            crew_proxy->setData(crew_idx.sibling(crew_idx.row(), PlaneCrewItem::Plane_Crew_Ability_Ignores_Deflections), arg);
        });
    }

    // Ability 'Unrestricted Maneuvers' handling. Only create if crew is a pilot or co-pilot
    if (crew_role_id == PlaneCrewItem::Pilot || crew_role_id == PlaneCrewItem::CoPilot) {
        QCheckBox* has_unrestricted_maneuvers = new QCheckBox("Unrestricted Maneuvers", layout->parentWidget());
        layout->addWidget(has_unrestricted_maneuvers, row++, 1);
        connect(has_unrestricted_maneuvers, &QCheckBox::clicked, this, [crew_idx, this](bool arg){
            crew_proxy->setData(crew_idx.sibling(crew_idx.row(), PlaneCrewItem::Plane_Crew_Ability_Unrestricted_Maneuvers), arg);
        });
    }

    // Convenience lambda to prep a vertical line for the firebase table
    auto vert_line = [](QWidget* parent){
        QFrame* line = new QFrame(parent);
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);
        return line;
    };

    auto label = [](const QString &text, QWidget* parent) {
        QLabel* label = new QLabel(text, parent);
        label->setAlignment(Qt::AlignCenter);
        return label;
    };

    // Iterate over the crew guns (if any) and build the UI components to display them
    for (int i=0; i<crew_proxy->rowCount(crew_idx); ++i) {
        QPersistentModelIndex gun_idx = crew_proxy->index(i, PlaneArmamentsItem::Plane_Armaments_Name, crew_idx);
        // Groupbox containing the firebase values
        QGroupBox* gun_grp_box = new QGroupBox(gun_idx.data().toString(), layout->parentWidget());
        QGridLayout* grid = new QGridLayout(gun_grp_box);
        gun_grp_box->setLayout(grid);
        int r=0, c=0; // init row and column counter for grid content positioning

        // Headers
        grid->addWidget(new QLabel("Range", gun_grp_box), r, c++);
        grid->addWidget(vert_line(gun_grp_box),  r, c++);
        grid->addWidget(label("3", gun_grp_box), r, c++);
        grid->addWidget(vert_line(gun_grp_box),  r, c++);
        grid->addWidget(label("2", gun_grp_box), r, c++);
        grid->addWidget(vert_line(gun_grp_box),  r, c++);
        grid->addWidget(label("1", gun_grp_box), r, c++);
        grid->addWidget(vert_line(gun_grp_box),  r, c++);
        grid->addWidget(label("0", gun_grp_box), r++, c++);

        // Horizontal Separator
        QFrame* line = new QFrame(grid->parentWidget());
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        grid->addWidget(line, r++, 0, 1, ++c);
        c=0; // reset column counter

        // Values
        grid->addWidget(new QLabel("Fire Base", gun_grp_box), r, c++);
        grid->addWidget(vert_line(gun_grp_box), r, c++);
        grid->addWidget(label(gun_idx.sibling(i, PlaneArmamentsItem::Plane_Armaments_Fire_Base_3).data().toString(), gun_grp_box), r, c++);
        grid->addWidget(vert_line(gun_grp_box), r, c++);
        grid->addWidget(label(gun_idx.sibling(i, PlaneArmamentsItem::Plane_Armaments_Fire_Base_2).data().toString(), gun_grp_box), r, c++);
        grid->addWidget(vert_line(gun_grp_box), r, c++);
        grid->addWidget(label(gun_idx.sibling(i, PlaneArmamentsItem::Plane_Armaments_Fire_Base_1).data().toString(), gun_grp_box), r, c++);
        grid->addWidget(vert_line(gun_grp_box), r, c++);
        grid->addWidget(label(gun_idx.sibling(i, PlaneArmamentsItem::Plane_Armaments_Fire_Base_0).data().toString(), gun_grp_box), r, c);

        // Add the firebase grid to the crew grid
        layout->addWidget(gun_grp_box, row++, 1, 1, 2);
    }
}
