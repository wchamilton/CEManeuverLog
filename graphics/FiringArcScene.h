#ifndef FIRINGARCSCENE_H
#define FIRINGARCSCENE_H

#include <QObject>
#include <QGraphicsScene>
#include <QPersistentModelIndex>

class FiringArc;
class PlaneFilterProxy;
class FiringArcScene : public QGraphicsScene
{
    Q_OBJECT
public:
    FiringArcScene(QObject *parent = nullptr);
    void setCurrentGun(QPersistentModelIndex idx);
    void setGunRotation(int pos);

private:
    void setFiringArc(int arc);
    void initFiringArcHexes(int arc);

    FiringArc* firing_arc = nullptr;
    QPersistentModelIndex gun_idx;
};

#endif // FIRINGARCSCENE_H
