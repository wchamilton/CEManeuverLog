#ifndef GUNROTATIONCONTROL_H
#define GUNROTATIONCONTROL_H

#include <QWidget>
#include <QPersistentModelIndex>

namespace Ui {
class GunRotationControl;
}

class FilterProxy;
class FiringArcScene;

class GunRotationControl : public QWidget
{
    Q_OBJECT

public:
    explicit GunRotationControl(QSharedPointer<FilterProxy> crew_proxy, QPersistentModelIndex gun_idx, QWidget *parent = nullptr);
    ~GunRotationControl();

private slots:
    void rotateGun(const int &delta);

private:
    Ui::GunRotationControl *ui;

    QSharedPointer<FilterProxy> crew_proxy;
    QPersistentModelIndex gun_idx;
    FiringArcScene* firing_arc_scene = nullptr;
};

#endif // GUNROTATIONCONTROL_H
