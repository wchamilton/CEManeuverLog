#ifndef STATUSEFFECTSINTERFACE_H
#define STATUSEFFECTSINTERFACE_H

#include <QWidget>
#include <QPersistentModelIndex>

namespace Ui {
class StatusEffectsInterface;
}

class FilterProxy;
class StatusEffectsInterface : public QWidget
{
    Q_OBJECT

public:
    explicit StatusEffectsInterface(QSharedPointer<FilterProxy> crew_proxy, QSharedPointer<FilterProxy> effects_proxy, QPersistentModelIndex base_plane_idx, QWidget *parent = nullptr);
    ~StatusEffectsInterface();

private slots:
    void handleEffectSelection(bool checked);

private:
    Ui::StatusEffectsInterface *ui;
    QSharedPointer<FilterProxy> crew_proxy;
    QSharedPointer<FilterProxy> effects_proxy;
    QPersistentModelIndex base_plane_idx;
};

#endif // STATUSEFFECTSINTERFACE_H
