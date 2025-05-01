#include "GameModel.h"
#include "CEManeuvers.h"
#include "GameModelItems.h"

#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <algorithm>

int col_count = 0;

GameModel::GameModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    // Determine how many columns there are
    std::vector<int> col_counts = { PlaneItem::PLANE_COL_COUNT, PlaneManeuverItem::PLANE_MANEUVER_COL_COUNT,
                                   PlaneArmamentsItem::PLANE_ARMAMENTS_COL_COUNT, PlaneCrewItem::PLANE_CREW_COLS,
                                   GameItem::GAME_ITEM_COLS, TurnItem::TURN_PLANE_COL_COUNT, TurnCrewItem::TURN_CREW_COL_COUNT,
                                   TurnArmamentItem::TURN_CREW_ARMAMENT_COL_COUNT };
    col_count = *std::max_element(col_counts.begin(), col_counts.end());

    // Create tree root
    root = new BaseItem(BaseItem::Base_Item_Type);

    // Create main branches
    GameItem* game_item = new GameItem(root);
    BaseItem* planes_branch = new BaseItem(BaseItem::Base_Item_Type, root);
    BaseItem* turns_branch =  new BaseItem(BaseItem::Base_Item_Type, root);

    // Add each of them as children of the main root node
    root->addChild(game_item);
    root->addChild(planes_branch);
    root->addChild(turns_branch);

    // Populate planes from Planes folder
    QDir planes_dir(PLANES_LOCATION);
    for (const QFileInfo &file_info : planes_dir.entryInfoList({"*.json"})) {
        QString file_path = file_info.absoluteFilePath();
        QFile file(file_path);
        if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
            qWarning() << "Could not open" << file_path;
            return;
        }
        QJsonDocument planes_doc = QJsonDocument::fromJson(QString(file.readAll()).toUtf8());
        file.close();

        planes_branch->addChild(new PlaneItem(planes_doc.object(), planes_branch));
    }
}

GameModel::~GameModel()
{
    delete root;
}

QModelIndex GameModel::index(int row, int column, const QModelIndex &parent) const
{
    BaseItem* parent_item = parent.isValid() ? static_cast<BaseItem*>(parent.internalPointer()) : root;
    if (row < parent_item->childCount() && row >= 0) {
        return createIndex(row, column, parent_item->childAt(row));
    }
    return QModelIndex();
}

QModelIndex GameModel::parent(const QModelIndex &idx) const
{
    if (!idx.isValid()) {
        return QModelIndex();
    }
    BaseItem* item = static_cast<BaseItem*>(idx.internalPointer());
    BaseItem* parent_item = item->getParent();

    if (parent_item == nullptr || parent_item == root) {
        return QModelIndex();
    }

    return createIndex(parent_item->row(), 0, parent_item);
}

int GameModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return root->childCount();
    }

    BaseItem* item = static_cast<BaseItem*>(parent.internalPointer());
    return item->childCount();
}

int GameModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return col_count; // It's stupid but check the docs.. this is just how it's done
}

QVariant GameModel::data(const QModelIndex &idx, int role) const
{
    if (!idx.isValid()) {
        return QVariant();
    }

    BaseItem* item = static_cast<BaseItem*>(idx.internalPointer());
    if (role == Qt::UserRole) {
        return item->getType();
    }
    if (role == Qt::DisplayRole) {
        return item->data(idx.column());
    }
    return QVariant();
}

bool GameModel::setData(const QModelIndex &idx, const QVariant &value, int role)
{
    if (!idx.isValid()) {
        return false;
    }
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        BaseItem* item = static_cast<BaseItem*>(idx.internalPointer());
        item->setData(idx.column(), value);
        emit dataChanged(idx, idx, {role});
        return true;
    }
    return false;
}

QPersistentModelIndex GameModel::addPlane()
{
    QModelIndex planes_root_idx = planesRootIdx();
    BaseItem* planes_root_ptr = static_cast<BaseItem*>(planes_root_idx.internalPointer());

    beginInsertRows(planes_root_idx, rowCount(planes_root_idx), rowCount(planes_root_idx)+1);
    PlaneItem* item = new PlaneItem(planes_root_ptr);
    planes_root_ptr->addChild(item);
    endInsertRows();
    return index(rowCount(planes_root_idx)-1, 0, planes_root_idx);
}

QPersistentModelIndex GameModel::addManeuver(const Maneuver &m, const QModelIndex &parent_plane_idx)
{
    BaseItem* plane_ptr = static_cast<BaseItem*>(parent_plane_idx.internalPointer());

    beginInsertRows(parent_plane_idx, rowCount(parent_plane_idx), rowCount(parent_plane_idx)+1);
    PlaneManeuverItem* maneuver_ptr = new PlaneManeuverItem(m, plane_ptr);
    plane_ptr->addChild(maneuver_ptr);
    endInsertRows();
    return index(rowCount(parent_plane_idx)-1, 0, parent_plane_idx);
}

QPersistentModelIndex GameModel::addCrew(const QModelIndex &parent_plane_idx)
{
    BaseItem* plane_ptr = static_cast<BaseItem*>(parent_plane_idx.internalPointer());

    beginInsertRows(parent_plane_idx, rowCount(parent_plane_idx), rowCount(parent_plane_idx)+1);
    PlaneCrewItem* crew_ptr = new PlaneCrewItem(plane_ptr);
    plane_ptr->addChild(crew_ptr);
    endInsertRows();
    return index(rowCount(parent_plane_idx)-1, 0, parent_plane_idx);
}

QPersistentModelIndex GameModel::addGun(const QModelIndex &parent_crew_idx)
{
    BaseItem* plane_ptr = static_cast<BaseItem*>(parent_crew_idx.internalPointer());

    beginInsertRows(parent_crew_idx, rowCount(parent_crew_idx), rowCount(parent_crew_idx)+1);
    PlaneArmamentsItem* gun_ptr = new PlaneArmamentsItem(plane_ptr);
    plane_ptr->addChild(gun_ptr);
    endInsertRows();
    return index(rowCount(parent_crew_idx)-1, 0, parent_crew_idx);
}

void GameModel::removeManeuver(const QModelIndex &idx)
{
    BaseItem* plane_ptr = static_cast<BaseItem*>(idx.parent().internalPointer());
    BaseItem* maneuver_ptr = static_cast<BaseItem*>(idx.internalPointer());

    beginRemoveRows(idx.parent(), idx.row(), idx.row());
    plane_ptr->removeChild(maneuver_ptr->row());
    endRemoveRows();
}

void GameModel::removeChild(int row, const QModelIndex &parent)
{
    BaseItem* parent_ptr = static_cast<BaseItem*>(parent.internalPointer());
    beginRemoveRows(parent, row, row);
    parent_ptr->removeChild(row);
    endRemoveRows();
}

void GameModel::removeChildren(const QModelIndex &parent)
{
    BaseItem* parent_ptr = static_cast<BaseItem*>(parent.internalPointer());
    beginRemoveRows(parent, 0, rowCount(parent));
    parent_ptr->removeChildren();
    endRemoveRows();
}

QJsonObject GameModel::dumpPlaneToJson(const QModelIndex &plane_idx)
{
    PlaneItem* plane_ptr = static_cast<PlaneItem*>(plane_idx.internalPointer());
    return plane_ptr->toJSON();
}

QModelIndex GameModel::gameRootIdx() const
{
    return index(Game_Root, 0);
}

QModelIndex GameModel::planesRootIdx() const
{
    return index(Planes_Root, 0);
}

QModelIndex GameModel::turnsRootIdx() const
{
    return index(Turns_Root, 0);
}

QModelIndex GameModel::currentTurn(int column) const
{
    QModelIndex turns_root = turnsRootIdx();
    return index(rowCount(turns_root)-1, column, turns_root);
}

void GameModel::applyActiveEffect(int effect, int duration, QString desc)
{
    QModelIndex plane_effects_idx = index(Planes_Root, PlaneItem::Plane_Active_Effects);
    QList effects = plane_effects_idx.data().toList();
    PlaneItem::Effect e = { effect, duration, desc };
    effects.removeAll(QVariant::fromValue(e));
    effects << QVariant::fromValue(e);
    setData(plane_effects_idx, effects);
}

FilterProxy::FilterProxy(QAbstractItemModel *src_model, QObject *parent) : QSortFilterProxyModel(parent)
{
    setSourceModel(src_model);
}

void FilterProxy::setTypeFilter(int type)
{
    item_type_filter.clear();
    item_type_filter << type;
    invalidate();
}

void FilterProxy::setTypeFilter(QList<int> types)
{
    item_type_filter = types;
    invalidate();
}

bool FilterProxy::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    return item_type_filter.contains(sourceModel()->index(source_row, 0, source_parent).data(Qt::UserRole).toInt());
}
