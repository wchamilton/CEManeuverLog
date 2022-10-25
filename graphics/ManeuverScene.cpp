#include "ManeuverScene.h"

#include <QtSvg/QGraphicsSvgItem>
#include <QPersistentModelIndex>
#include <QDebug>

#include "ManeuverGraphic.h"
#include "models/PlaneItems.h"
#include "CEManeuvers.h"

ManeuverScene::ManeuverScene(QObject *parent) : QGraphicsScene(parent)
{
    for (auto maneuver : master_maneuver_list) {
        ManeuverGraphic::ShiftText shift_val = ManeuverGraphic::Shift_None;
        if (maneuver.name == "1L0" || maneuver.name == "9L2" || maneuver.name == "11L2" || maneuver.name == "28R1") {
            shift_val = ManeuverGraphic::Shift_Left;
        }
        else if (maneuver.name == "1S0" || maneuver.name == "1R0"  || maneuver.name == "0S1" ||
                 maneuver.name == "9R2" || maneuver.name == "11R2" || maneuver.name == "28L1") {
            shift_val = ManeuverGraphic::Shift_Right;
        }
        maneuver_map[maneuver.name] = new ManeuverGraphic(shift_val);
        for (int i=0; i<maneuver.tile_movements.size()-1; ++i) {
            maneuver_map[maneuver.name]->addHex(maneuver.tile_movements.at(i));
        }
        maneuver_map[maneuver.name]->addHex(maneuver.tile_movements.last(),
                                  maneuver.name == "0S1" ? HexTile::Spin_Tile : HexTile::Destination_Tile,
                                  maneuver.final_rotation);
        addItem(maneuver_map[maneuver.name]);
    }
}

ManeuverGraphic *ManeuverScene::getManeuver(QString maneuver_name)
{
    return maneuver_map[maneuver_name];
}

QPersistentModelIndex ManeuverScene::getSelectedManeuverIdx()
{
    return selectedItems().isEmpty() ? QPersistentModelIndex() : static_cast<ManeuverGraphic*>(selectedItems().first())->getIdx();
}

QString ManeuverScene::getSelectedManeuver()
{
    return selectedItems().isEmpty() ? QString() : maneuver_map.key(static_cast<ManeuverGraphic*>(selectedItems().first()));
}

void ManeuverScene::addManeuver(QPersistentModelIndex maneuver_idx)
{
    maneuver_map[maneuver_idx.sibling(maneuver_idx.row(), ManeuverItem::Maneuver_Name).data().toString()]->setModelIndex(maneuver_idx);
}

void ManeuverScene::removeManeuver(QPersistentModelIndex maneuver_idx)
{
    maneuver_map[maneuver_idx.sibling(maneuver_idx.row(), ManeuverItem::Maneuver_Name).data().toString()]->setModelIndex(QPersistentModelIndex());
}

void ManeuverScene::setManeuver(QPersistentModelIndex maneuver_idx)
{
    for (auto key : maneuver_map.keys()) {
        maneuver_map[key]->setModelIndex(maneuver_idx.sibling(maneuver_idx.row(),
            ManeuverItem::Maneuver_Name).data().toString() == key ? maneuver_idx : QPersistentModelIndex());
    }
}

void ManeuverScene::setManeuvers(QPersistentModelIndex plane_idx)
{
    const QAbstractItemModel* plane_model = plane_idx.model();
    QMap<QString, QPersistentModelIndex> maneuver_indexes;

    if (plane_idx.isValid()) {
        // Iterate over the child items of the plane index
        for (int i=0; i<plane_model->rowCount(plane_idx); ++i) {
            QPersistentModelIndex index = plane_model->index(i, ManeuverItem::Maneuver_Name, plane_idx);
            QString name = index.data().toString();
            maneuver_indexes[name] = index;
        }
        plane_name->setPlainText(plane_idx.data().toString());
        plane_name->setX(background_item->boundingRect().width()/2 - plane_name->boundingRect().width()/2);
        plane_tolerances->setPlainText(QString("Dive: %1 | Climb: %2 | Altitude: %3%4 | Stability: %5")
                                       .arg(plane_idx.sibling(plane_idx.row(), PlaneItem::Rated_Dive).data().toString())
                                       .arg(plane_idx.sibling(plane_idx.row(), PlaneItem::Rated_Climb).data().toString())
                                       .arg(plane_idx.sibling(plane_idx.row(), PlaneItem::Max_Altitude).data().toString())
                                       .arg(plane_idx.sibling(plane_idx.row(), PlaneItem::Can_Return_To_Max_Alt).data().toBool() ? "+" : "")
                                       .arg(plane_idx.sibling(plane_idx.row(), PlaneItem::Stability).data().toString()));
        plane_tolerances->setX(background_item->boundingRect().width()/2 - plane_tolerances->boundingRect().width()/2);
    }

    for (auto key : maneuver_map.keys()) {
        maneuver_map[key]->setModelIndex(maneuver_indexes.keys().contains(key) ? maneuver_indexes[key] : QPersistentModelIndex());
    }
    update();
}

void ManeuverScene::updateManeuver(QString id)
{
    if (maneuver_map.keys().contains(id)) {
        maneuver_map[id]->updateManeuverState();
    }
}

void ManeuverScene::applyScheduleBG()
{
    // Add the background as a pixmap
//    QGraphicsSvgItem* background_item = new QGraphicsSvgItem(QString("../CEManeuverLog/graphics/Canvas_AircraftSheet.svg"));
//    background_item->setZValue(-1);
//    addItem(background_item);

    // for now use shitty png while debugging SVG
    background_item = addPixmap(QPixmap("../CEManeuverLog/graphics/background.png"));
    background_item->setZValue(-1);
    plane_name = addText("");
    plane_tolerances = addText("");
    QFont font = plane_name->font();
    font.setPixelSize(16);
    font.setBold(true);
    plane_name->setFont(font);
    font.setPixelSize(14);
    plane_tolerances->setFont(font);
    plane_name->moveBy(background_item->boundingRect().width()/2 - plane_name->boundingRect().width()/2, background_item->boundingRect().height());
    plane_tolerances->moveBy(background_item->boundingRect().width()/2 - plane_tolerances->boundingRect().width()/2, background_item->boundingRect().height() + plane_name->boundingRect().height());
}

void ManeuverScene::positionManeuvers()
{
    maneuver_map["1L0"]->moveBy(175, 35);
    maneuver_map["1S0"]->moveBy(275, 35);
    maneuver_map["1R0"]->moveBy(395, 35);
    maneuver_map["0S1"]->moveBy(388, 529);
    maneuver_map["10L1"]->moveBy(130, 148);
    maneuver_map["8L1"]->moveBy(180, 135);
    maneuver_map["6S1"]->moveBy(247, 130);
    maneuver_map["2S1"]->moveBy(292, 130);
    maneuver_map["7S1"]->moveBy(338, 130);
    maneuver_map["8R1"]->moveBy(385, 133);
    maneuver_map["10R1"]->moveBy(435, 148);
    maneuver_map["9L2"]->moveBy(30, 370);
    maneuver_map["17L2"]->moveBy(65, 320);
    maneuver_map["16L2"]->moveBy(100, 285);
    maneuver_map["15L2"]->moveBy(135, 270);
    maneuver_map["14L2"]->moveBy(180, 250);
    maneuver_map["11L2"]->moveBy(35, 220);
    maneuver_map["12S2"]->moveBy(247, 245);
    maneuver_map["3S2"]->moveBy(292, 245);
    maneuver_map["13S2"]->moveBy(338, 245);
    maneuver_map["9R2"]->moveBy(540, 370);
    maneuver_map["17R2"]->moveBy(510, 320);
    maneuver_map["16R2"]->moveBy(475, 285);
    maneuver_map["15R2"]->moveBy(435, 270);
    maneuver_map["14R2"]->moveBy(390, 250);
    maneuver_map["11R2"]->moveBy(540, 220);
    maneuver_map["24L3"]->moveBy(55, 530);
    maneuver_map["23L3"]->moveBy(85, 475);
    maneuver_map["22L3"]->moveBy(115, 440);
    maneuver_map["21L3"]->moveBy(150, 410);
    maneuver_map["20L3"]->moveBy(185, 385);
    maneuver_map["18S3"]->moveBy(247, 377);
    maneuver_map["4S3"]->moveBy(292, 377);
    maneuver_map["19S3"]->moveBy(338, 377);
    maneuver_map["20R3"]->moveBy(387, 385);
    maneuver_map["21R3"]->moveBy(422, 410);
    maneuver_map["22R3"]->moveBy(452, 440);
    maneuver_map["23R3"]->moveBy(482, 475);
    maneuver_map["24R3"]->moveBy(515, 530);
    maneuver_map["25S4"]->moveBy(247, 531);
    maneuver_map["5S4"]->moveBy(292, 531);
    maneuver_map["26S4"]->moveBy(338, 531);

    // Restricted speed 1
    maneuver_map["28L1"]->moveBy(115, 610);
    maneuver_map["28R1"]->moveBy(455, 610);

    // Restricted speed 2
    maneuver_map["30L2"]->moveBy(125, 650);
    maneuver_map["31L2"]->moveBy(185, 670);
    maneuver_map["29S2"]->moveBy(291, 650);
    maneuver_map["31R2"]->moveBy(385, 670);
    maneuver_map["30R2"]->moveBy(445, 650);

    // Restricted speed 3
    maneuver_map["36L3"]->moveBy(140, 770);
    maneuver_map["34S3"]->moveBy(230, 780);
    maneuver_map["32S3"]->moveBy(270, 780);
    maneuver_map["33S3"]->moveBy(310, 780);
    maneuver_map["35S3"]->moveBy(350, 780);
    maneuver_map["36R3"]->moveBy(430, 770);
}
