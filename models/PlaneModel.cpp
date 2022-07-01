#include "PlaneModel.h"
#include <QJsonArray>

PlaneModel::PlaneModel(QJsonArray planes, QObject *parent)
    : QAbstractItemModel(parent)
{
    root = new BaseItem();
    for (int i=0; i<planes.size(); ++i) {
        root->addChild(new PlaneItem(planes.at(i).toObject(), root));
    }
}

PlaneModel::~PlaneModel()
{
    delete root;
}

QModelIndex PlaneModel::index(int row, int column, const QModelIndex &parent) const
{
    BaseItem* parent_item = parent.isValid() ? static_cast<BaseItem*>(parent.internalPointer()) : root;
    if (row < parent_item->childCount() && row >= 0) {
        return createIndex(row, column, parent_item->childAt(row));
    }
    return QModelIndex();
}

QModelIndex PlaneModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }
    BaseItem* item = static_cast<BaseItem*>(index.internalPointer());
    BaseItem* parent_item = item->getParent();

    if (parent_item == nullptr || parent_item == root) {
        return QModelIndex();
    }

    return createIndex(parent_item->row(), 0, parent_item);
}

int PlaneModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return root->childCount();
    }

    BaseItem* item = static_cast<BaseItem*>(parent.internalPointer());
    return item->childCount();
}

int PlaneModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    return PlaneItem::COL_COUNT;
}

QVariant PlaneModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        BaseItem* item = static_cast<BaseItem*>(index.internalPointer());
        return item->data(index.column());
    }

    return QVariant();
}

bool PlaneModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) {
        return false;
    }
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        BaseItem* item = static_cast<BaseItem*>(index.internalPointer());
        item->setData(index.column(), value);
    }
    return true;
}
