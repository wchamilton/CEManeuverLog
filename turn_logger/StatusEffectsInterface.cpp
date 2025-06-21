#include "StatusEffectsInterface.h"
#include "ui_StatusEffectsInterface.h"

#include "models/GameModel.h"

#include <QStandardItemModel>

#define EFFECT_ROLE Qt::UserRole + 1
#define GUN_ROLE Qt::UserRole + 2

// Function provided by Parker Coates via stack overflow comment at https://stackoverflow.com/a/62261745
void setComboBoxItemEnabled(QComboBox * comboBox, int index, bool enabled)
{
    auto * model = qobject_cast<QStandardItemModel*>(comboBox->model());
    assert(model);
    if(!model) return;

    auto * item = model->item(index);
    assert(item);
    if(!item) return;
    item->setEnabled(enabled);
}

StatusEffectsInterface::StatusEffectsInterface(QSharedPointer<FilterProxy> crew_proxy, QPersistentModelIndex plane_idx, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StatusEffectsInterface)
{
    ui->setupUi(this);

    // Initiallize each of the combobox elements and initialize the effect structures at the same time as custom data
    auto addEffectOption = [=](PlaneItem::Effect effect) {
        ui->effect_selection_cmb->addItem(effect.name, QVariant::fromValue(effect));
    };

    addEffectOption({PlaneItem::Effect_Rudder_Jam_Left, 2, "Rudder jam (Left - 2 turns)", "Rudder is jammed left for 2 turns."});
    addEffectOption({PlaneItem::Effect_Rudder_Jam_Left, 3, "Rudder jam (Left - 3 turns)", "Rudder is jammed left for 3 turns."});
    addEffectOption({PlaneItem::Effect_Rudder_Jam_Right, 2, "Rudder jam (Right - 2 turns)", "Rudder is jammed right for 2 turns."});
    addEffectOption({PlaneItem::Effect_Rudder_Jam_Right, 2, "Rudder jam (Right - 3 turns)", "Rudder is jammed right for 3 turns."});
    addEffectOption({PlaneItem::Effect_Fuel_Tank_Hit_3, -1, "Fuel tank hit! (-3 fuel)", "Fuel tank was hit, 3 fuel was lost."});
    addEffectOption({PlaneItem::Effect_Fuel_Tank_Hit_6, -1, "Fuel tank hit! (-6 fuel)", "Fuel tank was hit, 6 fuel was lost."});
    addEffectOption({PlaneItem::Effect_Smoking, -1, "Smoking", "Plane is now smoking."});
    addEffectOption({PlaneItem::Effect_On_Fire, -1, "Fire!", "Plane is now on fire (Take one blue chit of damage each turn until put out)."});
    addEffectOption({PlaneItem::Effect_No_Restricted_Maneuvers, -1, "No restricted maneuvers", "Cannot perform restricted maneuvers for the duration of the match."});
    addEffectOption({PlaneItem::Effect_No_Speed_3_or_4, -1, "No speed 3 or 4 maneuvers", "Cannot perform speed 3 or 4 maneuvers for the duration of the match."});
    addEffectOption({PlaneItem::Effect_Force_Slower_Maneuver, 1, "Must do slower maneuver", "Must perform a slower maneuver next turn."});

    // Iterate over the crew's guns and add them to the dropdown as destructable. Only add them if they're still working!
    for (int crew_row = 0; crew_row<crew_proxy->rowCount(plane_idx); ++crew_row) {
        QModelIndex crew_idx = crew_proxy->index(crew_row, 0, plane_idx);
        for (int gun_row = 0; gun_row<crew_proxy->rowCount(crew_idx); ++gun_row) {
            QModelIndex gun_idx = crew_proxy->index(gun_row, 0, crew_idx);
            if (gun_idx.data(Qt::UserRole).toInt() == BaseItem::Plane_Armaments_Item_Type &&
                !gun_idx.sibling(gun_row, PlaneArmamentsItem::Plane_Armaments_Gun_Destroyed).data().toBool()) {
                ui->effect_selection_cmb->addItem(QString("%1 (link %2) was destroyed").arg(gun_idx.data().toString()).arg(gun_row+1), gun_idx);
            }
            else if (gun_idx.data(Qt::UserRole).toInt() == BaseItem::Plane_Armaments_Link_Item_Type) {
                for (int linked_gun_row = 0; linked_gun_row < crew_proxy->rowCount(gun_idx); ++linked_gun_row) {
                    QModelIndex linked_gun_idx = crew_proxy->index(linked_gun_row, 0, gun_idx);
                    if (linked_gun_idx.data(Qt::UserRole).toInt() == BaseItem::Plane_Armaments_Item_Type &&
                        !linked_gun_idx.sibling(linked_gun_row, PlaneArmamentsItem::Plane_Armaments_Gun_Destroyed).data().toBool()) {
                        ui->effect_selection_cmb->addItem(QString("%1 (link %2) was destroyed").arg(linked_gun_idx.data().toString()).arg(linked_gun_row+1), linked_gun_idx);
                    }
                }
            }
        }
    }

    connect(ui->add_effect_btn, &QPushButton::clicked, this, &StatusEffectsInterface::addStatusEffectToList);
}

StatusEffectsInterface::~StatusEffectsInterface()
{
    delete ui;
}

void StatusEffectsInterface::updateStatusList()
{

}

void StatusEffectsInterface::addStatusEffectToList()
{
    // Create an empty effect struct, this will be extracted or constructed after determining if it's a gun to pop
    PlaneItem::Effect effect;

    QVariant selected_effect_data = ui->effect_selection_cmb->currentData();
    // If we can convert it, we know it's some other effect
    if (selected_effect_data.canConvert<PlaneItem::Effect>()) {
        effect = selected_effect_data.value<PlaneItem::Effect>();
    }
    // If we can't convert it to an Effect, we assume it's a QModelIndex of a gun to destroy. First populate an Effect object that will get stored for viewing.
    else {
        effect.id = PlaneItem::Effect_Gun_Destroyed;
        effect.name = ui->effect_selection_cmb->currentText();
        effect.desc = "This gun was destroyed, it cannot be used for the remainder of the match.";
        effect.remaining_turns = -1; // -1 indicates perpetual
    }

    // If the effect can be overridden, remove the existing one and insert the new one
    removeStatusEffectFromList(effect);

    // Create a new list widget item to populate. Assigning the listwidget as the parent auto inserts it
    QListWidgetItem* item = new QListWidgetItem(effect.name, ui->effects_list);
    item->setData(EFFECT_ROLE, QVariant::fromValue(effect));

    // Extra logic for if we're popping a gun
    if (effect.id == PlaneItem::Effect_Gun_Destroyed) {
        setComboBoxItemEnabled(ui->effect_selection_cmb, ui->effect_selection_cmb->currentIndex(), false);
        item->setData(GUN_ROLE, selected_effect_data); // Store the gun index
    }

    // Set the tooltip, including the remaining turns (if it isn't perpetual)
    item->setToolTip(effect.desc + (effect.remaining_turns == -1 ? "" : QString(" - (%1 turns remain)").arg(effect.remaining_turns)));
}

void StatusEffectsInterface::removeStatusEffectFromList(PlaneItem::Effect effect)
{
    for (int i=0; i<ui->effects_list->count(); ++i) {
        if (ui->effects_list->item(i)->data(EFFECT_ROLE).value<PlaneItem::Effect>() == effect) {
            delete ui->effects_list->takeItem(i);
            return;
        }
    }
}
