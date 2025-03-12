#include "GameModel.h"
#include "CEManeuvers.h"
#include "GameModelItems.h"

#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

GameModel::GameModel(QObject *parent)
    : QAbstractItemModel(parent)
{
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

    return 17; // It's stupid but check the docs.. this is just how it's done
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

    return item->data(idx.column());
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

QModelIndex GameModel::currentTurn(int column) const
{
    QModelIndex turns_root = index(Turns_Root, 0);
    return index(rowCount(turns_root)-1, column, turns_root);
}

void GameModel::applyActiveEffect(int effect, int duration, QString desc)
{
    QList effects = index(Planes_Root, PlaneItem::Plane_Active_Effects).data().toList();
    PlaneItem::Effect e = { effect, duration, desc };
    effects.removeAll(QVariant::fromValue(e));
    effects << QVariant::fromValue(e);
    setData(index(Planes_Root, PlaneItem::Plane_Active_Effects), effects);
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
