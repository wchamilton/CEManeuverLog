#include "PlaneModel.h"
#include <QJsonArray>

PlaneModel::PlaneModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    root = new BaseItem(BaseItem::Base_Item_Type);
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
    return PlaneItem::COL_COUNT;
}

Qt::ItemFlags PlaneModel::flags(const QModelIndex &idx) const
{
    if (idx.sibling(idx.row(), 0).data(Qt::UserRole).toInt() == BaseItem::Maneuver_Item_Type &&
            !idx.sibling(idx.row(), ManeuverItem::IsEnabled).data().toBool()) {
        return QAbstractItemModel::flags(idx) & ~(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    }
    return QAbstractItemModel::flags(idx);
}

QVariant PlaneModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    BaseItem* item = static_cast<BaseItem*>(index.internalPointer());
    if (role == Qt::DisplayRole) {
        return item->data(index.column());
    }
    else if (role == Qt::UserRole) {
        return item->getType();
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
        emit dataChanged(index, index, {role});
    }
    return true;
}

void PlaneModel::loadPlanesJSON(QJsonArray planes)
{
    for (int i=0; i<planes.size(); ++i) {
        root->addChild(new PlaneItem(planes.at(i).toObject(), root));
    }
}

void PlaneModel::prepareTemplateModel()
{
    root->addChild(new PlaneItem(root));
}

PlaneFilterProxy::PlaneFilterProxy(PlaneModel *src_model, QObject *parent) : QSortFilterProxyModel(parent)
{
    setSourceModel(src_model);
}

void PlaneFilterProxy::setTypeFilter(BaseItem::ItemType type)
{
    type_filter = type;
    invalidate();
}

bool PlaneFilterProxy::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    return sourceModel()->index(source_row, 0, source_parent).data(Qt::UserRole).toInt() == type_filter ||
            sourceModel()->index(source_row, 0, source_parent).data(Qt::UserRole).toInt() == BaseItem::Plane_Item_Type; // Need to allow parent
}

bool PlaneFilterProxy::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    // Only do this logic chain if both items are maneuver items
    if (source_left.data(Qt::UserRole).toInt() == BaseItem::Maneuver_Item_Type &&
        source_right.data(Qt::UserRole).toInt() == BaseItem::Maneuver_Item_Type) {

        // Restricted maneuvers should be displayed later than regular ones
        if (!source_left.sibling(source_left.row(), ManeuverItem::Is_Restricted).data().toBool() &&
                source_right.sibling(source_right.row(), ManeuverItem::Is_Restricted).data().toBool()) {
            return true;
        }
        return false;
    }
    return QSortFilterProxyModel::lessThan(source_left, source_right);
}
