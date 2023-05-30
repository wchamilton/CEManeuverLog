#include "FiringArcScene.h"

#include <QPersistentModelIndex>

#include "FiringArc.h"
#include "HexTile.h"
#include "CEManeuvers.h"
#include "models/PlaneModel.h"

FiringArcScene::FiringArcScene(QObject *parent) : QGraphicsScene(parent)
{
}

void FiringArcScene::setCurrentGun(QPersistentModelIndex idx)
{
    gun_idx = idx.sibling(idx.row(), GunItem::Gun_Position);
    setFiringArc(gun_idx.sibling(gun_idx.row(), GunItem::Fire_Template).data().toInt());
    setGunRotation(gun_idx.data().toInt());
    update();
}

void FiringArcScene::setGunRotation(int pos)
{
    firing_arc->setGunPos(pos);
    update();
}

void FiringArcScene::setFiringArc(int arc)
{
    if (firing_arc) {
        removeItem(firing_arc);
        delete firing_arc;
    }
    firing_arc = new FiringArc(QString("Fire Template %1").arg(arc));
    initFiringArcHexes(arc);
    addItem(firing_arc);
}

void FiringArcScene::initFiringArcHexes(int arc)
{
    switch (arc) {
    case 1: {
        // Arc 1 is just a line ahead
        firing_arc->addHex({1}, new HexTile(HexTile::All_Tile), { Maneuver::North });
        firing_arc->addHex({1}, new HexTile(HexTile::All_Tile), { Maneuver::North, Maneuver::North });
        firing_arc->addHex({1}, new HexTile(HexTile::All_Tile), { Maneuver::North, Maneuver::North, Maneuver::North });
        break;
    }
    case 2: {
        // Range 1 hexes
        firing_arc->addHex({3}, new HexTile(HexTile::All_Tile), { Maneuver::South_East });
        firing_arc->addHex({4}, new HexTile(HexTile::Blind_Spot_1), { Maneuver::South });
        firing_arc->addHex({5}, new HexTile(HexTile::All_Tile), { Maneuver::South_West });

        // Range 2 hexes
        firing_arc->addHex({3}, new HexTile(HexTile::All_Tile), { Maneuver::South_East, Maneuver::South_East });
        firing_arc->addHex({3,4}, new HexTile(HexTile::All_Tile), { Maneuver::South_East, Maneuver::South });
        firing_arc->addHex({4}, new HexTile(HexTile::H_S_Tile), { Maneuver::South, Maneuver::South });
        firing_arc->addHex({4,5}, new HexTile(HexTile::All_Tile), { Maneuver::South, Maneuver::South_West });
        firing_arc->addHex({5}, new HexTile(HexTile::All_Tile), { Maneuver::South_West, Maneuver::South_West });

        // Range 3 hexes
        firing_arc->addHex({2,3}, new HexTile(HexTile::All_Tile), { Maneuver::North_East, Maneuver::South_East, Maneuver::South_East });
        firing_arc->addHex({3}, new HexTile(HexTile::All_Tile), { Maneuver::South_East, Maneuver::South_East, Maneuver::South_East });
        firing_arc->addHex({3,4}, new HexTile(HexTile::All_Tile), { Maneuver::South_East, Maneuver::South_East, Maneuver::South });
        firing_arc->addHex({3,4}, new HexTile(HexTile::H_S_Tile), { Maneuver::South_East, Maneuver::South, Maneuver::South });
        firing_arc->addHex({4}, new HexTile(HexTile::H_S_Tile), { Maneuver::South, Maneuver::South, Maneuver::South });
        firing_arc->addHex({4,5}, new HexTile(HexTile::H_S_Tile), { Maneuver::South_West, Maneuver::South, Maneuver::South });
        firing_arc->addHex({4,5}, new HexTile(HexTile::All_Tile), { Maneuver::South_West, Maneuver::South_West, Maneuver::South });
        firing_arc->addHex({5}, new HexTile(HexTile::All_Tile), { Maneuver::South_West, Maneuver::South_West, Maneuver::South_West });
        firing_arc->addHex({5,6}, new HexTile(HexTile::All_Tile), { Maneuver::South_West, Maneuver::South_West, Maneuver::North_West });
        break;
    }
    case 3: {
        // Range 1 hexes
        firing_arc->addHex({1}, new HexTile(HexTile::H_Tile), { Maneuver::North });
        firing_arc->addHex({2}, new HexTile(HexTile::H_Tile), { Maneuver::North_East });
        firing_arc->addHex({3}, new HexTile(HexTile::All_Tile), { Maneuver::South_East });
        firing_arc->addHex({4}, new HexTile(HexTile::Blind_Spot_1), { Maneuver::South });
        firing_arc->addHex({5}, new HexTile(HexTile::All_Tile), { Maneuver::South_West });
        firing_arc->addHex({6}, new HexTile(HexTile::H_Tile), { Maneuver::North_West });

        // Range 2 hexes
        firing_arc->addHex({1}, new HexTile(HexTile::H_Tile), { Maneuver::North, Maneuver::North });
        firing_arc->addHex({1,2}, new HexTile(HexTile::H_Tile), { Maneuver::North, Maneuver::North_East });
        firing_arc->addHex({2}, new HexTile(HexTile::H_Tile), { Maneuver::North_East, Maneuver::North_East });
        firing_arc->addHex({2,3}, new HexTile(HexTile::All_Tile), { Maneuver::North_East, Maneuver::South_East });
        firing_arc->addHex({3}, new HexTile(HexTile::All_Tile), { Maneuver::South_East, Maneuver::South_East });
        firing_arc->addHex({3,4}, new HexTile(HexTile::All_Tile), { Maneuver::South_East, Maneuver::South });
        firing_arc->addHex({4}, new HexTile(HexTile::H_S_Tile), { Maneuver::South, Maneuver::South });
        firing_arc->addHex({4,5}, new HexTile(HexTile::All_Tile), { Maneuver::South, Maneuver::South_West });
        firing_arc->addHex({5}, new HexTile(HexTile::All_Tile), { Maneuver::South_West, Maneuver::South_West });
        firing_arc->addHex({5,6}, new HexTile(HexTile::All_Tile), { Maneuver::South_West, Maneuver::North_West });
        firing_arc->addHex({6}, new HexTile(HexTile::H_Tile), { Maneuver::North_West, Maneuver::North_West });
        firing_arc->addHex({1,6}, new HexTile(HexTile::H_Tile), { Maneuver::North_West, Maneuver::North });

        // Range 3 hexes
        firing_arc->addHex({1}, new HexTile(HexTile::H_Tile), { Maneuver::North, Maneuver::North, Maneuver::North });
        firing_arc->addHex({1,2}, new HexTile(HexTile::H_Tile), { Maneuver::North, Maneuver::North, Maneuver::North_East });
        firing_arc->addHex({1,2}, new HexTile(HexTile::H_Tile), { Maneuver::North, Maneuver::North_East, Maneuver::North_East });
        firing_arc->addHex({2}, new HexTile(HexTile::H_Tile), { Maneuver::North_East, Maneuver::North_East, Maneuver::North_East });
        firing_arc->addHex({2,3}, new HexTile(HexTile::H_Tile), { Maneuver::North_East, Maneuver::North_East, Maneuver::South_East });
        firing_arc->addHex({2,3}, new HexTile(HexTile::All_Tile), { Maneuver::North_East, Maneuver::South_East, Maneuver::South_East });
        firing_arc->addHex({3}, new HexTile(HexTile::All_Tile), { Maneuver::South_East, Maneuver::South_East, Maneuver::South_East });
        firing_arc->addHex({3,4}, new HexTile(HexTile::All_Tile), { Maneuver::South_East, Maneuver::South_East, Maneuver::South });
        firing_arc->addHex({3,4}, new HexTile(HexTile::H_S_Tile), { Maneuver::South_East, Maneuver::South, Maneuver::South });
        firing_arc->addHex({4}, new HexTile(HexTile::H_S_Tile), { Maneuver::South, Maneuver::South, Maneuver::South });
        firing_arc->addHex({4,5}, new HexTile(HexTile::H_S_Tile), { Maneuver::South_West, Maneuver::South, Maneuver::South });
        firing_arc->addHex({4,5}, new HexTile(HexTile::All_Tile), { Maneuver::South_West, Maneuver::South_West, Maneuver::South });
        firing_arc->addHex({5}, new HexTile(HexTile::All_Tile), { Maneuver::South_West, Maneuver::South_West, Maneuver::South_West });
        firing_arc->addHex({5,6}, new HexTile(HexTile::All_Tile), { Maneuver::South_West, Maneuver::South_West, Maneuver::North_West });
        firing_arc->addHex({5,6}, new HexTile(HexTile::H_Tile), { Maneuver::South_West, Maneuver::North_West, Maneuver::North_West });
        firing_arc->addHex({6}, new HexTile(HexTile::H_Tile), { Maneuver::North_West, Maneuver::North_West, Maneuver::North_West });
        firing_arc->addHex({1,6}, new HexTile(HexTile::H_Tile), { Maneuver::North_West, Maneuver::North_West, Maneuver::North });
        firing_arc->addHex({1,6}, new HexTile(HexTile::H_Tile), { Maneuver::North_West, Maneuver::North, Maneuver::North });
        break;
    }
    case 4: {
        // Range 1 hexes
        firing_arc->addHex({1}, new HexTile(HexTile::H_Tile), { Maneuver::North });
        firing_arc->addHex({2}, new HexTile(HexTile::H_S_Tile), { Maneuver::North_East });
        firing_arc->addHex({3}, new HexTile(HexTile::All_Tile), { Maneuver::South_East });
        firing_arc->addHex({4}, new HexTile(HexTile::Blind_Spot_1), { Maneuver::South });
        firing_arc->addHex({5}, new HexTile(HexTile::All_Tile), { Maneuver::South_West });
        firing_arc->addHex({6}, new HexTile(HexTile::H_S_Tile), { Maneuver::North_West });

        // Range 2 hexes
        firing_arc->addHex({1}, new HexTile(HexTile::H_Tile), { Maneuver::North, Maneuver::North });
        firing_arc->addHex({1,2}, new HexTile(HexTile::H_S_Tile), { Maneuver::North, Maneuver::North_East });
        firing_arc->addHex({2}, new HexTile(HexTile::H_S_Tile), { Maneuver::North_East, Maneuver::North_East });
        firing_arc->addHex({2,3}, new HexTile(HexTile::All_Tile), { Maneuver::North_East, Maneuver::South_East });
        firing_arc->addHex({3}, new HexTile(HexTile::All_Tile), { Maneuver::South_East, Maneuver::South_East });
        firing_arc->addHex({3,4}, new HexTile(HexTile::All_Tile), { Maneuver::South_East, Maneuver::South });
        firing_arc->addHex({4}, new HexTile(HexTile::H_S_Tile), { Maneuver::South, Maneuver::South });
        firing_arc->addHex({4,5}, new HexTile(HexTile::All_Tile), { Maneuver::South, Maneuver::South_West });
        firing_arc->addHex({5}, new HexTile(HexTile::All_Tile), { Maneuver::South_West, Maneuver::South_West });
        firing_arc->addHex({5,6}, new HexTile(HexTile::All_Tile), { Maneuver::South_West, Maneuver::North_West });
        firing_arc->addHex({6}, new HexTile(HexTile::H_S_Tile), { Maneuver::North_West, Maneuver::North_West });
        firing_arc->addHex({1,6}, new HexTile(HexTile::H_S_Tile), { Maneuver::North_West, Maneuver::North });

        // Range 3 hexes
        firing_arc->addHex({1}, new HexTile(HexTile::H_Tile), { Maneuver::North, Maneuver::North, Maneuver::North });
        firing_arc->addHex({1,2}, new HexTile(HexTile::H_Tile), { Maneuver::North, Maneuver::North, Maneuver::North_East });
        firing_arc->addHex({1,2}, new HexTile(HexTile::H_S_Tile), { Maneuver::North, Maneuver::North_East, Maneuver::North_East });
        firing_arc->addHex({2}, new HexTile(HexTile::H_S_Tile), { Maneuver::North_East, Maneuver::North_East, Maneuver::North_East });
        firing_arc->addHex({2,3}, new HexTile(HexTile::H_S_Tile), { Maneuver::North_East, Maneuver::North_East, Maneuver::South_East });
        firing_arc->addHex({2,3}, new HexTile(HexTile::All_Tile), { Maneuver::North_East, Maneuver::South_East, Maneuver::South_East });
        firing_arc->addHex({3}, new HexTile(HexTile::All_Tile), { Maneuver::South_East, Maneuver::South_East, Maneuver::South_East });
        firing_arc->addHex({3,4}, new HexTile(HexTile::All_Tile), { Maneuver::South_East, Maneuver::South_East, Maneuver::South });
        firing_arc->addHex({3,4}, new HexTile(HexTile::H_S_Tile), { Maneuver::South_East, Maneuver::South, Maneuver::South });
        firing_arc->addHex({4}, new HexTile(HexTile::H_S_Tile), { Maneuver::South, Maneuver::South, Maneuver::South });
        firing_arc->addHex({4,5}, new HexTile(HexTile::H_S_Tile), { Maneuver::South_West, Maneuver::South, Maneuver::South });
        firing_arc->addHex({4,5}, new HexTile(HexTile::All_Tile), { Maneuver::South_West, Maneuver::South_West, Maneuver::South });
        firing_arc->addHex({5}, new HexTile(HexTile::All_Tile), { Maneuver::South_West, Maneuver::South_West, Maneuver::South_West });
        firing_arc->addHex({5,6}, new HexTile(HexTile::All_Tile), { Maneuver::South_West, Maneuver::South_West, Maneuver::North_West });
        firing_arc->addHex({5,6}, new HexTile(HexTile::H_S_Tile), { Maneuver::South_West, Maneuver::North_West, Maneuver::North_West });
        firing_arc->addHex({6}, new HexTile(HexTile::H_S_Tile), { Maneuver::North_West, Maneuver::North_West, Maneuver::North_West });
        firing_arc->addHex({1,6}, new HexTile(HexTile::H_S_Tile), { Maneuver::North_West, Maneuver::North_West, Maneuver::North });
        firing_arc->addHex({1,6}, new HexTile(HexTile::H_Tile), { Maneuver::North_West, Maneuver::North, Maneuver::North });
        break;
    }
    case 5: {
        // Range 1 hexes
        firing_arc->addHex({1}, new HexTile(HexTile::All_Tile), { Maneuver::North });

        // Range 2 hexes
        firing_arc->addHex({1}, new HexTile(HexTile::All_Tile), { Maneuver::North, Maneuver::North });
        firing_arc->addHex({1}, new HexTile(HexTile::All_Tile), { Maneuver::North, Maneuver::North_East });
        firing_arc->addHex({1}, new HexTile(HexTile::All_Tile), { Maneuver::North_West, Maneuver::North });

        // Range 3 hexes
        firing_arc->addHex({1}, new HexTile(HexTile::All_Tile), { Maneuver::North, Maneuver::North, Maneuver::North });
        firing_arc->addHex({1}, new HexTile(HexTile::All_Tile), { Maneuver::North, Maneuver::North, Maneuver::North_East });
        firing_arc->addHex({1}, new HexTile(HexTile::All_Tile), { Maneuver::North, Maneuver::North_East, Maneuver::North_East });
        firing_arc->addHex({1}, new HexTile(HexTile::All_Tile), { Maneuver::North_West, Maneuver::North_West, Maneuver::North });
        firing_arc->addHex({1}, new HexTile(HexTile::All_Tile), { Maneuver::North_West, Maneuver::North, Maneuver::North });
        break;
    }
    case 6: {
        // Range 1 hexes
        firing_arc->addHex({1}, new HexTile(HexTile::H_S_Tile), { Maneuver::North });

        // Range 2 hexes
        firing_arc->addHex({1}, new HexTile(HexTile::H_S_Tile), { Maneuver::North, Maneuver::North });
        firing_arc->addHex({1}, new HexTile(HexTile::H_S_Tile), { Maneuver::North, Maneuver::North_East });
        firing_arc->addHex({1}, new HexTile(HexTile::H_S_Tile), { Maneuver::North_West, Maneuver::North });

        // Range 3 hexes
        firing_arc->addHex({1}, new HexTile(HexTile::H_S_Tile), { Maneuver::North, Maneuver::North, Maneuver::North });
        firing_arc->addHex({1}, new HexTile(HexTile::H_S_Tile), { Maneuver::North, Maneuver::North, Maneuver::North_East });
        firing_arc->addHex({1}, new HexTile(HexTile::H_S_Tile), { Maneuver::North, Maneuver::North_East, Maneuver::North_East });
        firing_arc->addHex({1}, new HexTile(HexTile::H_S_Tile), { Maneuver::North_West, Maneuver::North_West, Maneuver::North });
        firing_arc->addHex({1}, new HexTile(HexTile::H_S_Tile), { Maneuver::North_West, Maneuver::North, Maneuver::North });
        break;
    }
    case 7: {
        // Range 1 hexes
        firing_arc->addHex({1}, new HexTile(HexTile::All_Tile), { Maneuver::North });
        firing_arc->addHex({2}, new HexTile(HexTile::All_Tile), { Maneuver::North_East });
        firing_arc->addHex({6}, new HexTile(HexTile::All_Tile), { Maneuver::North_West });

        // Range 2 hexes
        firing_arc->addHex({1}, new HexTile(HexTile::All_Tile), { Maneuver::North, Maneuver::North });
        firing_arc->addHex({1,2}, new HexTile(HexTile::All_Tile), { Maneuver::North, Maneuver::North_East });
        firing_arc->addHex({2}, new HexTile(HexTile::All_Tile), { Maneuver::North_East, Maneuver::North_East });
        firing_arc->addHex({6}, new HexTile(HexTile::All_Tile), { Maneuver::North_West, Maneuver::North_West });
        firing_arc->addHex({1,6}, new HexTile(HexTile::All_Tile), { Maneuver::North_West, Maneuver::North });

        // Range 3 hexes
        firing_arc->addHex({1}, new HexTile(HexTile::All_Tile), { Maneuver::North, Maneuver::North, Maneuver::North });
        firing_arc->addHex({1,2}, new HexTile(HexTile::All_Tile), { Maneuver::North, Maneuver::North, Maneuver::North_East });
        firing_arc->addHex({1,2}, new HexTile(HexTile::All_Tile), { Maneuver::North, Maneuver::North_East, Maneuver::North_East });
        firing_arc->addHex({2}, new HexTile(HexTile::All_Tile), { Maneuver::North_East, Maneuver::North_East, Maneuver::North_East });
        firing_arc->addHex({6}, new HexTile(HexTile::All_Tile), { Maneuver::North_West, Maneuver::North_West, Maneuver::North_West });
        firing_arc->addHex({1,6}, new HexTile(HexTile::All_Tile), { Maneuver::North_West, Maneuver::North_West, Maneuver::North });
        firing_arc->addHex({1,6}, new HexTile(HexTile::All_Tile), { Maneuver::North_West, Maneuver::North, Maneuver::North });
        break;
    }
    case 8: {
        // Range 1 hexes
        firing_arc->addHex({3}, new HexTile(HexTile::H_Tile), { Maneuver::South_East });
        firing_arc->addHex({4}, new HexTile(HexTile::H_Tile), { Maneuver::South });
        firing_arc->addHex({5}, new HexTile(HexTile::H_Tile), { Maneuver::South_West });

        // Range 2 hexes
        firing_arc->addHex({3}, new HexTile(HexTile::H_Tile), { Maneuver::South_East, Maneuver::South_East });
        firing_arc->addHex({3,4}, new HexTile(HexTile::H_Tile), { Maneuver::South_East, Maneuver::South });
        firing_arc->addHex({4}, new HexTile(HexTile::H_Tile), { Maneuver::South, Maneuver::South });
        firing_arc->addHex({4,5}, new HexTile(HexTile::H_Tile), { Maneuver::South, Maneuver::South_West });
        firing_arc->addHex({5}, new HexTile(HexTile::H_Tile), { Maneuver::South_West, Maneuver::South_West });

        // Range 3 hexes
        firing_arc->addHex({3}, new HexTile(HexTile::H_Tile), { Maneuver::South_East, Maneuver::South_East, Maneuver::South_East });
        firing_arc->addHex({3,4}, new HexTile(HexTile::H_Tile), { Maneuver::South_East, Maneuver::South_East, Maneuver::South });
        firing_arc->addHex({3,4}, new HexTile(HexTile::H_Tile), { Maneuver::South_East, Maneuver::South, Maneuver::South });
        firing_arc->addHex({4}, new HexTile(HexTile::H_Tile), { Maneuver::South, Maneuver::South, Maneuver::South });
        firing_arc->addHex({4,5}, new HexTile(HexTile::H_Tile), { Maneuver::South_West, Maneuver::South, Maneuver::South });
        firing_arc->addHex({4,5}, new HexTile(HexTile::H_Tile), { Maneuver::South_West, Maneuver::South_West, Maneuver::South });
        firing_arc->addHex({5}, new HexTile(HexTile::H_Tile), { Maneuver::South_West, Maneuver::South_West, Maneuver::South_West });
        break;
    }
    case 9: {
        // Range 1 hexes
        firing_arc->addHex({4}, new HexTile(HexTile::L_Tile), { Maneuver::South });

        // Range 2 hexes
        firing_arc->addHex({4}, new HexTile(HexTile::L_Tile), { Maneuver::South, Maneuver::South });

        // Range 3 hexes
        firing_arc->addHex({4}, new HexTile(HexTile::L_Tile), { Maneuver::South, Maneuver::South, Maneuver::South_West });
        firing_arc->addHex({4}, new HexTile(HexTile::L_Tile), { Maneuver::South, Maneuver::South, Maneuver::South });
        firing_arc->addHex({4}, new HexTile(HexTile::L_Tile), { Maneuver::South, Maneuver::South, Maneuver::South_East });
        break;
    }
    case 10: {
        // Range 1 hexes
        firing_arc->addHex({1}, new HexTile(HexTile::All_Tile), { Maneuver::North });
        firing_arc->addHex({2}, new HexTile(HexTile::All_Tile), { Maneuver::North_East });
        firing_arc->addHex({3}, new HexTile(HexTile::H_Tile), { Maneuver::South_East });
        firing_arc->addHex({4}, new HexTile(HexTile::H_Tile), { Maneuver::South });
        firing_arc->addHex({5}, new HexTile(HexTile::H_Tile), { Maneuver::South_West });
        firing_arc->addHex({6}, new HexTile(HexTile::All_Tile), { Maneuver::North_West });

        // Range 2 hexes
        firing_arc->addHex({1}, new HexTile(HexTile::All_Tile), { Maneuver::North, Maneuver::North });
        firing_arc->addHex({1,2}, new HexTile(HexTile::All_Tile), { Maneuver::North, Maneuver::North_East });
        firing_arc->addHex({2}, new HexTile(HexTile::All_Tile), { Maneuver::North_East, Maneuver::North_East });
        firing_arc->addHex({2,3}, new HexTile(HexTile::H_Tile), { Maneuver::North_East, Maneuver::South_East });
        firing_arc->addHex({3}, new HexTile(HexTile::H_Tile), { Maneuver::South_East, Maneuver::South_East });
        firing_arc->addHex({3,4}, new HexTile(HexTile::H_Tile), { Maneuver::South_East, Maneuver::South });
        firing_arc->addHex({4}, new HexTile(HexTile::H_Tile), { Maneuver::South, Maneuver::South });
        firing_arc->addHex({4,5}, new HexTile(HexTile::H_Tile), { Maneuver::South, Maneuver::South_West });
        firing_arc->addHex({5}, new HexTile(HexTile::H_Tile), { Maneuver::South_West, Maneuver::South_West });
        firing_arc->addHex({5,6}, new HexTile(HexTile::H_Tile), { Maneuver::South_West, Maneuver::North_West });
        firing_arc->addHex({6}, new HexTile(HexTile::All_Tile), { Maneuver::North_West, Maneuver::North_West });
        firing_arc->addHex({1,6}, new HexTile(HexTile::All_Tile), { Maneuver::North_West, Maneuver::North });

        // Range 3 hexes
        firing_arc->addHex({1}, new HexTile(HexTile::All_Tile), { Maneuver::North, Maneuver::North, Maneuver::North });
        firing_arc->addHex({1,2}, new HexTile(HexTile::All_Tile), { Maneuver::North, Maneuver::North, Maneuver::North_East });
        firing_arc->addHex({1,2}, new HexTile(HexTile::All_Tile), { Maneuver::North, Maneuver::North_East, Maneuver::North_East });
        firing_arc->addHex({2}, new HexTile(HexTile::All_Tile), { Maneuver::North_East, Maneuver::North_East, Maneuver::North_East });
        firing_arc->addHex({2,3}, new HexTile(HexTile::All_Tile), { Maneuver::North_East, Maneuver::North_East, Maneuver::South_East });
        firing_arc->addHex({2,3}, new HexTile(HexTile::H_Tile), { Maneuver::North_East, Maneuver::South_East, Maneuver::South_East });
        firing_arc->addHex({3}, new HexTile(HexTile::H_Tile), { Maneuver::South_East, Maneuver::South_East, Maneuver::South_East });
        firing_arc->addHex({3,4}, new HexTile(HexTile::H_Tile), { Maneuver::South_East, Maneuver::South_East, Maneuver::South });
        firing_arc->addHex({3,4}, new HexTile(HexTile::H_Tile), { Maneuver::South_East, Maneuver::South, Maneuver::South });
        firing_arc->addHex({4}, new HexTile(HexTile::H_Tile), { Maneuver::South, Maneuver::South, Maneuver::South });
        firing_arc->addHex({4,5}, new HexTile(HexTile::H_Tile), { Maneuver::South_West, Maneuver::South, Maneuver::South });
        firing_arc->addHex({4,5}, new HexTile(HexTile::H_Tile), { Maneuver::South_West, Maneuver::South_West, Maneuver::South });
        firing_arc->addHex({5}, new HexTile(HexTile::H_Tile), { Maneuver::South_West, Maneuver::South_West, Maneuver::South_West });
        firing_arc->addHex({5,6}, new HexTile(HexTile::H_Tile), { Maneuver::South_West, Maneuver::South_West, Maneuver::North_West });
        firing_arc->addHex({5,6}, new HexTile(HexTile::All_Tile), { Maneuver::South_West, Maneuver::North_West, Maneuver::North_West });
        firing_arc->addHex({6}, new HexTile(HexTile::All_Tile), { Maneuver::North_West, Maneuver::North_West, Maneuver::North_West });
        firing_arc->addHex({1,6}, new HexTile(HexTile::All_Tile), { Maneuver::North_West, Maneuver::North_West, Maneuver::North });
        firing_arc->addHex({1,6}, new HexTile(HexTile::All_Tile), { Maneuver::North_West, Maneuver::North, Maneuver::North });
        break;
    }
    case 11: {
        // Range 1 hexes
        firing_arc->addHex({1}, new HexTile(HexTile::H_S_Tile), { Maneuver::North });
        firing_arc->addHex({2}, new HexTile(HexTile::H_S_Tile), { Maneuver::North_East });
        firing_arc->addHex({3}, new HexTile(HexTile::All_Tile), { Maneuver::South_East });
        firing_arc->addHex({4}, new HexTile(HexTile::Blind_Spot_2), { Maneuver::South });
        firing_arc->addHex({5}, new HexTile(HexTile::All_Tile), { Maneuver::South_West });
        firing_arc->addHex({6}, new HexTile(HexTile::H_S_Tile), { Maneuver::North_West });

        // Range 2 hexes
        firing_arc->addHex({1}, new HexTile(HexTile::H_S_Tile), { Maneuver::North, Maneuver::North });
        firing_arc->addHex({1,2}, new HexTile(HexTile::H_S_Tile), { Maneuver::North, Maneuver::North_East });
        firing_arc->addHex({2}, new HexTile(HexTile::H_S_Tile), { Maneuver::North_East, Maneuver::North_East });
        firing_arc->addHex({2,3}, new HexTile(HexTile::H_S_Tile), { Maneuver::North_East, Maneuver::South_East });
        firing_arc->addHex({3}, new HexTile(HexTile::All_Tile), { Maneuver::South_East, Maneuver::South_East });
        firing_arc->addHex({3,4}, new HexTile(HexTile::All_Tile), { Maneuver::South_East, Maneuver::South });
        firing_arc->addHex({4}, new HexTile(HexTile::H_S_Tile), { Maneuver::South, Maneuver::South });
        firing_arc->addHex({4,5}, new HexTile(HexTile::All_Tile), { Maneuver::South, Maneuver::South_West });
        firing_arc->addHex({5}, new HexTile(HexTile::All_Tile), { Maneuver::South_West, Maneuver::South_West });
        firing_arc->addHex({5,6}, new HexTile(HexTile::H_S_Tile), { Maneuver::South_West, Maneuver::North_West });
        firing_arc->addHex({6}, new HexTile(HexTile::H_S_Tile), { Maneuver::North_West, Maneuver::North_West });
        firing_arc->addHex({1,6}, new HexTile(HexTile::H_S_Tile), { Maneuver::North_West, Maneuver::North });

        // Range 3 hexes
        firing_arc->addHex({1}, new HexTile(HexTile::H_S_Tile), { Maneuver::North, Maneuver::North, Maneuver::North });
        firing_arc->addHex({1,2}, new HexTile(HexTile::H_S_Tile), { Maneuver::North, Maneuver::North, Maneuver::North_East });
        firing_arc->addHex({1,2}, new HexTile(HexTile::H_S_Tile), { Maneuver::North, Maneuver::North_East, Maneuver::North_East });
        firing_arc->addHex({2}, new HexTile(HexTile::H_S_Tile), { Maneuver::North_East, Maneuver::North_East, Maneuver::North_East });
        firing_arc->addHex({2,3}, new HexTile(HexTile::H_S_Tile), { Maneuver::North_East, Maneuver::North_East, Maneuver::South_East });
        firing_arc->addHex({2,3}, new HexTile(HexTile::All_Tile), { Maneuver::North_East, Maneuver::South_East, Maneuver::South_East });
        firing_arc->addHex({3}, new HexTile(HexTile::All_Tile), { Maneuver::South_East, Maneuver::South_East, Maneuver::South_East });
        firing_arc->addHex({3,4}, new HexTile(HexTile::All_Tile), { Maneuver::South_East, Maneuver::South_East, Maneuver::South });
        firing_arc->addHex({3,4}, new HexTile(HexTile::H_S_Tile), { Maneuver::South_East, Maneuver::South, Maneuver::South });
        firing_arc->addHex({4}, new HexTile(HexTile::H_S_Tile), { Maneuver::South, Maneuver::South, Maneuver::South });
        firing_arc->addHex({4,5}, new HexTile(HexTile::H_S_Tile), { Maneuver::South_West, Maneuver::South, Maneuver::South });
        firing_arc->addHex({4,5}, new HexTile(HexTile::All_Tile), { Maneuver::South_West, Maneuver::South_West, Maneuver::South });
        firing_arc->addHex({5}, new HexTile(HexTile::All_Tile), { Maneuver::South_West, Maneuver::South_West, Maneuver::South_West });
        firing_arc->addHex({5,6}, new HexTile(HexTile::All_Tile), { Maneuver::South_West, Maneuver::South_West, Maneuver::North_West });
        firing_arc->addHex({5,6}, new HexTile(HexTile::H_S_Tile), { Maneuver::South_West, Maneuver::North_West, Maneuver::North_West });
        firing_arc->addHex({6}, new HexTile(HexTile::H_S_Tile), { Maneuver::North_West, Maneuver::North_West, Maneuver::North_West });
        firing_arc->addHex({1,6}, new HexTile(HexTile::H_S_Tile), { Maneuver::North_West, Maneuver::North_West, Maneuver::North });
        firing_arc->addHex({1,6}, new HexTile(HexTile::H_S_Tile), { Maneuver::North_West, Maneuver::North, Maneuver::North });
        break;
    }
    case 12: {
        // Range 1 hexes
        firing_arc->addHex({1}, new HexTile(HexTile::L_Tile), { Maneuver::North });

        // Range 2 hexes
        firing_arc->addHex({1}, new HexTile(HexTile::L_Tile), { Maneuver::North, Maneuver::North });

        // Range 3 hexes
        firing_arc->addHex({1}, new HexTile(HexTile::L_Tile), { Maneuver::North, Maneuver::North, Maneuver::North_East });
        firing_arc->addHex({1}, new HexTile(HexTile::L_Tile), { Maneuver::North, Maneuver::North, Maneuver::North });
        firing_arc->addHex({1}, new HexTile(HexTile::L_Tile), { Maneuver::North, Maneuver::North, Maneuver::North_West });
        break;
    }
    case 13: {
        // Range 1 hexes
        firing_arc->addHex({4}, new HexTile(HexTile::H_Tile), { Maneuver::South });

        // Range 2 hexes
        firing_arc->addHex({3,4}, new HexTile(HexTile::All_Tile), { Maneuver::South_East, Maneuver::South });
        firing_arc->addHex({4}, new HexTile(HexTile::H_Tile), { Maneuver::South, Maneuver::South });
        firing_arc->addHex({4,5}, new HexTile(HexTile::All_Tile), { Maneuver::South, Maneuver::South_West });

        // Range 3 hexes
        firing_arc->addHex({3,4}, new HexTile(HexTile::All_Tile), { Maneuver::South_East, Maneuver::South_East, Maneuver::South });
        firing_arc->addHex({3,4}, new HexTile(HexTile::All_Tile), { Maneuver::South_East, Maneuver::South, Maneuver::South });
        firing_arc->addHex({4}, new HexTile(HexTile::H_Tile), { Maneuver::South, Maneuver::South, Maneuver::South });
        firing_arc->addHex({4,5}, new HexTile(HexTile::All_Tile), { Maneuver::South_West, Maneuver::South, Maneuver::South });
        firing_arc->addHex({4,5}, new HexTile(HexTile::All_Tile), { Maneuver::South_West, Maneuver::South_West, Maneuver::South });
        break;
    }
    case 14: {
        // Range 1 hexes
        firing_arc->addHex({2}, new HexTile(HexTile::S_Tile), { Maneuver::North_East });
        firing_arc->addHex({6}, new HexTile(HexTile::S_Tile), { Maneuver::North_West });

        // Range 2 hexes
        firing_arc->addHex({2}, new HexTile(HexTile::All_Tile), { Maneuver::North_East, Maneuver::North_East });
        firing_arc->addHex({6}, new HexTile(HexTile::All_Tile), { Maneuver::North_West, Maneuver::North_West });

        // Range 3 hexes
        firing_arc->addHex({2}, new HexTile(HexTile::All_Tile), { Maneuver::North_East, Maneuver::North_East, Maneuver::North_East });
        firing_arc->addHex({2}, new HexTile(HexTile::All_Tile), { Maneuver::North_East, Maneuver::North_East, Maneuver::North });
        firing_arc->addHex({6}, new HexTile(HexTile::All_Tile), { Maneuver::North_West, Maneuver::North_West, Maneuver::North_West });
        firing_arc->addHex({6}, new HexTile(HexTile::All_Tile), { Maneuver::North_West, Maneuver::North_West, Maneuver::North });
        break;
    }
    case 15: {
        // Range 1 hexes
        firing_arc->addHex({5}, new HexTile(HexTile::All_Tile), { Maneuver::South_West });

        // Range 2 hexes
        firing_arc->addHex({5}, new HexTile(HexTile::All_Tile), { Maneuver::South_West, Maneuver::South_West });
        firing_arc->addHex({5}, new HexTile(HexTile::All_Tile), { Maneuver::South_West, Maneuver::North_West });

        // Range 3 hexes
        firing_arc->addHex({5}, new HexTile(HexTile::All_Tile), { Maneuver::South_West, Maneuver::South_West, Maneuver::South });
        firing_arc->addHex({5}, new HexTile(HexTile::All_Tile), { Maneuver::South_West, Maneuver::South_West, Maneuver::South_West });
        firing_arc->addHex({5}, new HexTile(HexTile::All_Tile), { Maneuver::South_West, Maneuver::South_West, Maneuver::North_West });
        break;
    }
    case 16: {
        // Range 1 hexes
        firing_arc->addHex({3}, new HexTile(HexTile::All_Tile), { Maneuver::South_East });

        // Range 2 hexes
        firing_arc->addHex({3}, new HexTile(HexTile::All_Tile), { Maneuver::North_East, Maneuver::South_East });
        firing_arc->addHex({3}, new HexTile(HexTile::All_Tile), { Maneuver::South_East, Maneuver::South_East });

        // Range 3 hexes
        firing_arc->addHex({3}, new HexTile(HexTile::All_Tile), { Maneuver::North_East, Maneuver::South_East, Maneuver::South_East });
        firing_arc->addHex({3}, new HexTile(HexTile::All_Tile), { Maneuver::South_East, Maneuver::South_East, Maneuver::South_East });
        firing_arc->addHex({3}, new HexTile(HexTile::All_Tile), { Maneuver::South_East, Maneuver::South_East, Maneuver::South });
        break;
    }
    default: return;
    }
}
