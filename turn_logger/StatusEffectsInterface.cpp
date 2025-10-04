#include "StatusEffectsInterface.h"
#include "ui_StatusEffectsInterface.h"

#include "models/GameModel.h"
#include "models/GameModelItems.h"

#define EFFECT_ROLE Qt::UserRole + 1
#define GUN_ROLE Qt::UserRole + 2

StatusEffectsInterface::StatusEffectsInterface(QSharedPointer<FilterProxy> crew_proxy, QSharedPointer<FilterProxy> effects_proxy, QPersistentModelIndex base_plane_idx, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StatusEffectsInterface),
    crew_proxy(crew_proxy),
    effects_proxy(effects_proxy),
    base_plane_idx(base_plane_idx)
{
    ui->setupUi(this);
    ui->effects_list->setModel(effects_proxy.data());
    ui->effects_list->setRootIndex(effects_proxy->mapFromSource(base_plane_idx));
    ui->effects_list->setModelColumn(PlaneEffectItem::Plane_Effect_Name);

    // Assign the effect information to the provided button, this will be used to populate a new effect item
    auto assignEffect = [=](QPushButton* btn, int id, const QString &name, const QString &desc, int turns) {
        btn->setProperty("effect_id", id);
        btn->setProperty("effect_name", name);
        btn->setProperty("effect_desc", desc);
        btn->setProperty("effect_duration", turns);
        connect(btn, &QPushButton::clicked, this, &StatusEffectsInterface::handleEffectSelection);
    };

    assignEffect(ui->no_restricted_maneuvers_btn, PlaneEffectItem::Effect_No_Restricted_Maneuvers, "No restricted maneuvers", "Cannot perform restricted maneuvers for the duration of the match.", -1);
    assignEffect(ui->no_3_4_maneuvers_btn, PlaneEffectItem::Effect_No_Speed_3_or_4, "No speed 3 or 4 maneuvers", "Cannot perform speed 3 or 4 maneuvers for the duration of the match.", -1);
    assignEffect(ui->force_slower_btn, PlaneEffectItem::Effect_Force_Slower_Maneuver, "Must do slower maneuver", "Must perform a slower maneuver.", 1);
    assignEffect(ui->gun_destroyed_btn, PlaneEffectItem::Effect_Gun_Destroyed, "Gun destroyed", "A gun was destroyed, it cannot be used for the remainder of the match.", -1);
    assignEffect(ui->rudder_jam_left_2_btn, PlaneEffectItem::Effect_Rudder_Jam_Left, "Rudder Jam Left", "Rudder is jammed left for %1 turn(s).", 2);
    assignEffect(ui->rudder_jam_left_3_btn, PlaneEffectItem::Effect_Rudder_Jam_Left, "Rudder Jam Left", "Rudder is jammed left for %1 turn(s).", 3);
    assignEffect(ui->rudder_jam_right_2_btn, PlaneEffectItem::Effect_Rudder_Jam_Right, "Rudder Jam Right", "Rudder is jammed right for %1 turn(s).", 2);
    assignEffect(ui->rudder_jam_right_3_btn, PlaneEffectItem::Effect_Rudder_Jam_Right, "Rudder Jam Right", "Rudder is jammed right for %1 turn(s).", 3);
    assignEffect(ui->fuel_tank_hit_3, PlaneEffectItem::Effect_Fuel_Tank_Hit_3, "Fuel tank hit! (-3 fuel)", "Fuel tank was hit, 3 fuel was lost.", -1);
    assignEffect(ui->fuel_tank_hit_6, PlaneEffectItem::Effect_Fuel_Tank_Hit_6, "Fuel tank hit! (-6 fuel)", "Fuel tank was hit, 6 fuel was lost.", -1);
    assignEffect(ui->smoking_btn, PlaneEffectItem::Effect_Smoking, "Smoking", "Plane is now smoking.", -1);
    assignEffect(ui->on_fire_btn, PlaneEffectItem::Effect_On_Fire, "Fire!", "Plane is now on fire (Take one blue chit of damage each turn until put out).", -1);
}

StatusEffectsInterface::~StatusEffectsInterface()
{
    delete ui;
}

void StatusEffectsInterface::handleEffectSelection(bool checked)
{
    // Spawn a new effect item
    auto createEffectItem = [this](QObject* sender) {
        QPersistentModelIndex effect_idx = effects_proxy->mapFromSource(static_cast<GameModel*>(effects_proxy->sourceModel())->addEffect(base_plane_idx));
        effects_proxy->setData(effect_idx.sibling(effect_idx.row(), PlaneEffectItem::Plane_Effect_ID), sender->property("effect_id"));
        effects_proxy->setData(effect_idx.sibling(effect_idx.row(), PlaneEffectItem::Plane_Effect_Name), sender->property("effect_name"));
        effects_proxy->setData(effect_idx.sibling(effect_idx.row(), PlaneEffectItem::Plane_Effect_Desc), sender->property("effect_desc"));
        effects_proxy->setData(effect_idx.sibling(effect_idx.row(), PlaneEffectItem::Plane_Effect_Remaining_Turns), sender->property("effect_duration"));
        return effect_idx;
    };

    auto removeEffectItem = [=](QModelIndex idx) {
        static_cast<GameModel*>(effects_proxy->sourceModel())->removeEffect(effects_proxy->mapToSource(idx));
    };

    auto findEffectItem = [=](int effect_id) {
        QModelIndex proxy_plane_idx = effects_proxy->mapFromSource(base_plane_idx);
        for (int effects_row=0; proxy_plane_idx.isValid() && effects_row<effects_proxy->rowCount(proxy_plane_idx); ++effects_row) {
            QModelIndex effect_index = effects_proxy->index(effects_row, PlaneEffectItem::Plane_Effect_ID, proxy_plane_idx);
            if (effect_index.data().toInt() == effect_id) {
                return effect_index;
            }
        }
        return QModelIndex();
    };

    int effect_ID = sender()->property("effect_id").toInt();
    switch(effect_ID) {
    case PlaneEffectItem::Effect_No_Restricted_Maneuvers:
    case PlaneEffectItem::Effect_No_Speed_3_or_4: {
        // These can only be applied a single time in a game but handle toggle because.. people will fuck up otherwise
        if (checked && !findEffectItem(effect_ID).isValid()) {
            createEffectItem(sender());
        }
        else if (!checked) {
            QModelIndex idx = findEffectItem(effect_ID);
            if (idx.isValid()) {
                removeEffectItem(idx);
            }
        }
        break;
    }
    case PlaneEffectItem::Effect_Force_Slower_Maneuver: {
        // These can only be applied a single time in a game
        if (!findEffectItem(effect_ID).isValid()) {
            createEffectItem(sender());
        }
        break;
    }
    case PlaneEffectItem::Effect_Gun_Destroyed: {
        // If the user wants to spam it, fine.
        // The end of turn logic will assign the destroyed state randomly however many times this has been clicked
        createEffectItem(sender());
        break;
    }
    case PlaneEffectItem::Effect_Rudder_Jam_Left:
    case PlaneEffectItem::Effect_Rudder_Jam_Right: {
        // Replace any existing jams
        QModelIndex idx = findEffectItem(PlaneEffectItem::Effect_Rudder_Jam_Left);
        if (idx.isValid()) {
            removeEffectItem(idx);
        }
        idx = findEffectItem(PlaneEffectItem::Effect_Rudder_Jam_Right);
        if (idx.isValid()) {
            removeEffectItem(idx);
        }
        createEffectItem(sender());
        break;
    }
    case PlaneEffectItem::Effect_Fuel_Tank_Hit_3:
    case PlaneEffectItem::Effect_Fuel_Tank_Hit_6: {
        // Fuel loss can always happen so always create the effect item
        createEffectItem(sender());
        break;
    }
    case PlaneEffectItem::Effect_Smoking: {
        if (checked && !findEffectItem(PlaneEffectItem::Effect_Smoking).isValid()) {
            QModelIndex idx = findEffectItem(PlaneEffectItem::Effect_On_Fire);
            if (idx.isValid()) {
                ui->on_fire_btn->setChecked(false);
                removeEffectItem(idx);
            }
            createEffectItem(sender());
        }
        else if (!checked) {
            QModelIndex idx = findEffectItem(PlaneEffectItem::Effect_Smoking);
            if (idx.isValid()) {
                removeEffectItem(idx);
            }
        }
        break;
    }
    case PlaneEffectItem::Effect_On_Fire: {
        if (checked && !findEffectItem(PlaneEffectItem::Effect_On_Fire).isValid()) {
            QModelIndex idx = findEffectItem(PlaneEffectItem::Effect_Smoking);
            if (idx.isValid()) {
                ui->smoking_btn->setChecked(false);
                removeEffectItem(idx);
            }
            createEffectItem(sender());
        }
        else if (!checked) {
            QModelIndex idx = findEffectItem(PlaneEffectItem::Effect_On_Fire);
            if (idx.isValid()) {
                removeEffectItem(idx);
            }
        }
        break;
    }
    default: break;
    }
    ui->effects_list->update();
}
