#include "PlaneEditor.h"
#include "ui_PlaneEditor.h"

#include <QToolButton>
#include <QInputDialog>
#include <QGraphicsItem>
#include <QPersistentModelIndex>
#include <QDataWidgetMapper>
#include <QVariant>
#include <QFileDialog>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTextCodec>

#include "CrewEditorTab.h"
#include "models/PlaneModel.h"
#include "graphics/ManeuverScene.h"
#include "graphics/ManeuverGraphic.h"

PlaneEditor::PlaneEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlaneEditor)
{
    ui->setupUi(this);
    initWidgets();

    // Add initial crew member to the tab and set the index accordingly
    int pilot_index = ui->crew_editor_tab->addTab(new CrewEditorTab(ui->crew_editor_tab), "Pilot");
    ui->crew_editor_tab->setCurrentIndex(pilot_index);
    ui->crew_editor_tab->tabBar()->tabButton(pilot_index, QTabBar::RightSide)->resize(0, 0);
}

PlaneEditor::PlaneEditor(const QJsonObject &plane_object, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlaneEditor)
{
    ui->setupUi(this);
    initWidgets();

    // Because this UI already has a plane model with a custom setup for maneuvers, we'll just populate the widgets
    ui->plane_name->setText(plane_object["name"].toString());
    if (plane_object["plane_era"].toString() == "Late War") {
        ui->late_war_btn->setChecked(true);
    }
    ui->fuel_amt->setValue(plane_object["fuel"].toInt());
    ui->engine_hp->setValue(plane_object["engine_hp"].toInt());
    ui->engine_critical_hp->setValue(plane_object["engine_critical"].toInt());
    ui->wing_hp->setValue(plane_object["wing_hp"].toInt());
    ui->wing_critical_hp->setValue(plane_object["wing_critical"].toInt());
    ui->fuselage_hp->setValue(plane_object["fuselage_hp"].toInt());
    ui->fuselage_critical_hp->setValue(plane_object["fuselage_critical"].toInt());
    ui->tail_hp->setValue(plane_object["tail_hp"].toInt());
    ui->tail_critical_hp->setValue(plane_object["tail_critical"].toInt());
    ui->rated_climb->setValue(plane_object["rated_climb"].toInt());
    ui->rated_dive->setValue(plane_object["rated_dive"].toInt());
    ui->max_alt->setText(plane_object["max_alt"].toString());
    ui->stab_rating->setText(plane_object["stability"].toString());

    // Populate the crew
    QJsonArray crew = plane_object["crew"].toArray();
    for (int i=0; i<crew.size(); ++i) {
        QJsonObject crew_item = crew.at(i).toObject();
        CrewEditorTab* tab = new CrewEditorTab(ui->crew_editor_tab);
        tab->populateFromJSON(crew_item);
        ui->crew_editor_tab->addTab(tab, crew_item["role"].toString());
    }

    // We'll always have had at least the pilot so we can safely do this
    ui->crew_editor_tab->setCurrentIndex(1);
    ui->crew_editor_tab->tabBar()->tabButton(1, QTabBar::RightSide)->resize(0, 0);

    // Update the template model with what was saved to file
    QJsonArray maneuvers = plane_object["maneuvers"].toArray();
    QModelIndex plane_idx = maneuver_proxy_model->index(0,0);
    for (int i=0; i<maneuvers.size(); ++i) {
        QJsonObject maneuver = maneuvers.at(i).toObject();
        QPersistentModelIndex idx(maneuver_proxy_model->index(ui->maneuver_selection->findText(maneuver["name"].toString()),
                                                              ManeuverItem::Maneuver_Name, plane_idx));
        auto setData = [&](int column, QVariant data) {
            maneuver_proxy_model->setData(idx.sibling(idx.row(), column), data);
        };

        QStringList tolerances = maneuver.value("tolerances").toString().split("/");
        setData(ManeuverItem::Climb_Value,          tolerances.at(0));
        setData(ManeuverItem::Level_Value,          tolerances.at(1));
        setData(ManeuverItem::Dive_Value,           tolerances.at(2));
        setData(ManeuverItem::Can_Be_Repeated,      maneuver.value("can_be_repeated").toVariant());
        setData(ManeuverItem::Can_Reload,           maneuver.value("can_reload").toVariant());
        setData(ManeuverItem::Can_Put_Out_Fires,    maneuver.value("can_put_out_fires").toVariant());
        setData(ManeuverItem::Observer_Can_Reload,  maneuver.value("observer_can_reload").toVariant());
        setData(ManeuverItem::Is_Restricted,        maneuver.value("is_restricted").toVariant());
        setData(ManeuverItem::Is_Weight_Restricted, maneuver.value("is_weight_restricted").toVariant());
        setData(ManeuverItem::Is_Climb_Restricted,  maneuver.value("is_climb_restricted").toVariant());
        setData(ManeuverItem::Causes_Spin_Check,    maneuver.value("causes_spin_check").toVariant());

        addManeuverToSchedule(idx);
    }
}

PlaneEditor::~PlaneEditor()
{
    delete ui;
}

void PlaneEditor::updatePreview(int row)
{
    QPersistentModelIndex idx(maneuver_proxy_model->index(row, ManeuverItem::Maneuver_Name, ui->maneuver_selection->rootModelIndex()));

    // Set the widgets with index contents
    ui->set_climb->setCurrentText(idx.sibling(row, ManeuverItem::Climb_Value).data().toString());
    ui->set_level->setCurrentText(idx.sibling(row, ManeuverItem::Level_Value).data().toString());
    ui->set_dive->setCurrentText(idx.sibling(row, ManeuverItem::Dive_Value).data().toString());
    ui->set_can_reload->setChecked(idx.sibling(row, ManeuverItem::Can_Reload).data().toBool());
    ui->set_obs_can_reload->setChecked(idx.sibling(row, ManeuverItem::Observer_Can_Reload).data().toBool());
    ui->set_can_put_out_fires->setChecked(idx.sibling(row, ManeuverItem::Can_Put_Out_Fires).data().toBool());
    ui->set_can_repeat->setChecked(idx.sibling(row, ManeuverItem::Can_Be_Repeated).data().toBool());
    ui->set_weight_restricted->setChecked(idx.sibling(row, ManeuverItem::Is_Weight_Restricted).data().toBool());

    maneuver_preview_scene->setManeuver(idx);
    ui->maneuver_preview_gv->fitInView(maneuver_preview_scene->getManeuver(idx.data().toString()), Qt::KeepAspectRatio);
}

void PlaneEditor::exportJSON()
{
    PlaneItem* plane = static_cast<PlaneItem*>(plane_model->index(0,0).internalPointer());

    plane->setData(PlaneItem::Plane_Name, ui->plane_name->text());
    plane->setData(PlaneItem::Plane_Era, ui->early_war_btn->isChecked() ? "Early War" : "Late War");
    plane->setData(PlaneItem::Fuel, ui->fuel_amt->value());
    plane->setData(PlaneItem::Engine_HP, ui->engine_hp->value());
    plane->setData(PlaneItem::Engine_Critical, ui->engine_critical_hp->value());
    plane->setData(PlaneItem::Wing_HP, ui->wing_hp->value());
    plane->setData(PlaneItem::Wing_Critical, ui->wing_critical_hp->value());
    plane->setData(PlaneItem::Fuselage_HP, ui->fuselage_hp->value());
    plane->setData(PlaneItem::Fuselage_Critical, ui->fuselage_critical_hp->value());
    plane->setData(PlaneItem::Tail_HP, ui->tail_hp->value());
    plane->setData(PlaneItem::Tail_Critical, ui->tail_critical_hp->value());
    plane->setData(PlaneItem::Rated_Climb, ui->rated_climb->value());
    plane->setData(PlaneItem::Rated_Dive, ui->rated_dive->value());
    plane->setData(PlaneItem::Max_Altitude, ui->max_alt->text());
    plane->setData(PlaneItem::Stability, ui->stab_rating->text());

    // Start at 1 since index 0 is used by the [+] tab
    for (int i=1; i<ui->crew_editor_tab->count(); ++i) {
        CrewItem* item = new CrewItem(plane);
        static_cast<CrewEditorTab*>(ui->crew_editor_tab->widget(i))->populateCrewItem(item);
        plane->addChild(item);
    }

    QSettings settings;
    QString planes_dir = "/home";
    if (settings.contains("planes_dir")) {
        planes_dir = settings.value("planes_dir").toString();
    }

    QString file_path = QFileDialog::getSaveFileName(this, tr("Save Plane"), planes_dir, tr("JSON files (*.json)"));
    if (file_path == "") {
        return;
    }

    QFileInfo file_info(file_path);
    if (file_info.completeSuffix() != "json") {
        file_info.setFile(file_path + ".json");
    }

    QFile file(file_info.absoluteFilePath());
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text)) {
        return;
    }

    // Ensure that if there are any unicode characters, they're preserved properly
    QTextStream out(&file);
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    out.setCodec(codec);
    QString configDoc = codec->fromUnicode(QString(QJsonDocument(plane_model->dumpPlaneToJSON(plane_model->index(0,0))).toJson()));

    out << configDoc;
    out.flush();
    file.close();
    settings.setValue("planes_dir", file_info.dir().absolutePath());
}

void PlaneEditor::initWidgets()
{
    // Create button to be placed in tabs row
    QToolButton *tb = new QToolButton(ui->crew_editor_tab);
    tb->setAutoRaise(true);
    tb->setText("+");

    // Add empty, not enabled tab to tabWidget
    ui->crew_editor_tab->addTab(new QLabel("Add tabs by pressing \"+\""), QString());
    ui->crew_editor_tab->setTabEnabled(0, false);

    // Add tab button to current tab. Button will be enabled, but tab -- not
    ui->crew_editor_tab->tabBar()->setTabButton(0, QTabBar::RightSide, tb);

    // Initialize the model and assign it to the components that are driven by it
    plane_model = new PlaneModel(this);
    plane_model->prepareTemplateModel();
    maneuver_proxy_model = new PlaneFilterProxy(plane_model, this);
    maneuver_proxy_model->expandFilter(BaseItem::Maneuver_Item_Type);

    ui->maneuver_selection->setModel(maneuver_proxy_model);
    ui->maneuver_selection->setRootModelIndex(maneuver_proxy_model->index(0,0)); // There's only one plane in this model
    ui->maneuver_selection->setCurrentIndex(-1);
    maneuver_proxy_model->sort(ManeuverItem::Speed);

    // Initialize the scenes and apply them to the graphics views
    maneuver_preview_scene = new ManeuverScene(ui->maneuver_preview_gv);
    maneuver_schedule_scene = new ManeuverScene(ui->maneuver_schedule_gv);
    maneuver_schedule_scene->positionManeuvers();
    ui->maneuver_preview_gv->setScene(maneuver_preview_scene);
    ui->maneuver_schedule_gv->setScene(maneuver_schedule_scene);
    ui->maneuver_preview_gv->setRenderHints(QPainter::Antialiasing);
    ui->maneuver_schedule_gv->setRenderHints(QPainter::Antialiasing);

    // Add and lock 0S1 since all planes need it
    int cmb_row = ui->maneuver_selection->findText("0S1");
    QPersistentModelIndex idx_0S1(maneuver_proxy_model->index(cmb_row, ManeuverItem::Maneuver_Name, ui->maneuver_selection->rootModelIndex()));
    maneuver_proxy_model->setData(idx_0S1.sibling(cmb_row, ManeuverItem::IsEnabled), false);
    addManeuverToSchedule(idx_0S1);

    // Setup connections
    connect(ui->set_climb, &QComboBox::currentTextChanged, this, [&](QString text) { setManeuverData(ManeuverItem::Climb_Value, text); });
    connect(ui->set_level, &QComboBox::currentTextChanged, this, [&](QString text) { setManeuverData(ManeuverItem::Level_Value, text); });
    connect(ui->set_dive, &QComboBox::currentTextChanged, this, [&](QString text) { setManeuverData(ManeuverItem::Dive_Value, text); });
    connect(ui->set_can_reload, &QCheckBox::toggled, this, [&](bool checked) { setManeuverData(ManeuverItem::Can_Reload, checked); });
    connect(ui->set_obs_can_reload, &QCheckBox::toggled, this, [&](bool checked) { setManeuverData(ManeuverItem::Observer_Can_Reload, checked); });
    connect(ui->set_can_put_out_fires, &QCheckBox::toggled, this, [&](bool checked) { setManeuverData(ManeuverItem::Can_Put_Out_Fires, checked); });
    connect(ui->set_can_repeat, &QCheckBox::toggled, this, [&](bool checked) { setManeuverData(ManeuverItem::Can_Be_Repeated, checked); });
    connect(ui->set_weight_restricted, &QCheckBox::toggled, this, [&](bool checked) {
        setManeuverData(ManeuverItem::Is_Weight_Restricted, checked);
        maneuver_preview_scene->updateManeuver(ui->maneuver_selection->currentText());
        maneuver_schedule_scene->updateManeuver(ui->maneuver_selection->currentText());
    });

    connect(ui->add_maneuver, &QPushButton::pressed, this, [&]() {
        QPersistentModelIndex idx(maneuver_proxy_model->index(ui->maneuver_selection->currentIndex(), ManeuverItem::Maneuver_Name, ui->maneuver_selection->rootModelIndex()));
        addManeuverToSchedule(idx);
    });

    connect(ui->remove_maneuver, &QPushButton::pressed, this, [&]() {
        QPersistentModelIndex idx(maneuver_proxy_model->index(ui->maneuver_selection->currentIndex(), ManeuverItem::Maneuver_Name, ui->maneuver_selection->rootModelIndex()));
        removeManeuverFromSchedule(idx);
    });

    connect(ui->export_json, &QPushButton::pressed, this, &PlaneEditor::exportJSON);

    connect(ui->maneuver_selection, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PlaneEditor::updatePreview);
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

    connect(maneuver_schedule_scene, &ManeuverScene::maneuverSelectionChanged, [&](QString name){
        if (name != "") {
            ui->maneuver_selection->setCurrentIndex(ui->maneuver_selection->findText(name));
        }
    });
}

void PlaneEditor::setManeuverData(int column, QVariant data)
{
    maneuver_proxy_model->setData(maneuver_proxy_model->index(ui->maneuver_selection->currentIndex(), column, ui->maneuver_selection->rootModelIndex()), data);
    maneuver_preview_scene->update();
    maneuver_schedule_scene->update();
}

void PlaneEditor::addManeuverToSchedule(QPersistentModelIndex idx)
{
    maneuver_schedule_scene->addManeuver(idx);
    maneuver_proxy_model->setData(idx.sibling(idx.row(), ManeuverItem::Added_To_Schedule), true);
}

void PlaneEditor::removeManeuverFromSchedule(QPersistentModelIndex idx)
{
    maneuver_schedule_scene->removeManeuver(idx);
    maneuver_proxy_model->setData(idx.sibling(idx.row(), ManeuverItem::Added_To_Schedule), false);
}
