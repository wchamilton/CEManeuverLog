#ifndef STATUSEFFECTSINTERFACE_H
#define STATUSEFFECTSINTERFACE_H

#include <QWidget>
#include <QPersistentModelIndex>
#include "models/GameModelItems.h"

namespace Ui {
class StatusEffectsInterface;
}

class FilterProxy;
class StatusEffectsInterface : public QWidget
{
    Q_OBJECT

public:
    explicit StatusEffectsInterface(QSharedPointer<FilterProxy> crew_proxy, QPersistentModelIndex plane_idx, QWidget *parent = nullptr);
    ~StatusEffectsInterface();
    void updateStatusList();

private slots:
    void addStatusEffectToList();
    void removeStatusEffectFromList(PlaneItem::Effect effect);

private:
    Ui::StatusEffectsInterface *ui;
    QSharedPointer<FilterProxy> crew_proxy;
    QPersistentModelIndex plane_idx;
};

#endif // STATUSEFFECTSINTERFACE_H
