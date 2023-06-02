#include "TurnModel.h"
#include "PlaneItems.h"

#include <QColor>
#include <QDebug>

TurnModel::TurnModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    root = new BaseItem(BaseItem::Base_Item_Type);
}

QModelIndex TurnModel::index(int row, int column, const QModelIndex &parent) const
{
    BaseItem* parent_item = parent.isValid() ? static_cast<BaseItem*>(parent.internalPointer()) : root;
    if (row < parent_item->childCount() && row >= 0) {
        return createIndex(row, column, parent_item->childAt(row));
    }
    return QModelIndex();
}

QModelIndex TurnModel::parent(const QModelIndex &index) const
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

int TurnModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return root->childCount();
    }

    BaseItem* item = static_cast<BaseItem*>(parent.internalPointer());
    return item->childCount();
}

int TurnModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return TurnItem::Turn_Item_Col_Count;
}

QVariant TurnModel::data(const QModelIndex &idx, int role) const
{
    if (!idx.isValid()) {
        return QVariant();
    }

    BaseItem* item = static_cast<BaseItem*>(idx.internalPointer());
    if (item->getType() == BaseItem::Turn_Item_Type) {
        switch (idx.column()) {
        case TurnItem::Turn_Number: {
            if (role == Qt::UserRole) {
                return idx.row()+1;
            }
            else if (role == Qt::DisplayRole) {
                return QString("Turn %1").arg(idx.row()+1);
            }
            break;
        }
        case TurnItem::Turn_Maneuver_Col: {
            if (role == Qt::UserRole) {
                return item->data(idx.column());
            }
            else if (role == Qt::DisplayRole) {
                QPersistentModelIndex maneuver_idx = item->data(idx.column()).toPersistentModelIndex();
                return QString("%1 - %2").arg(maneuver_idx.sibling(maneuver_idx.row(), ManeuverItem::Maneuver_Name).data().toString())
                                         .arg(item->data(TurnItem::Turn_Tolerance_Tag).toString());
            }
            break;
        }
        case TurnItem::Turn_Altitude_Col: {
            if (role == Qt::UserRole) {
                return item->data(idx.column());
            }
            else if (role == Qt::DisplayRole) {
                QPersistentModelIndex plane_idx = item->data(TurnItem::Turn_Maneuver_Col).toPersistentModelIndex().parent();
                return QString("%1/%2").arg(item->data(idx.column()).toInt()).arg(plane_idx.sibling(plane_idx.row(), PlaneItem::Max_Altitude).data().toInt());
            }
            break;
        }
        case TurnItem::Turn_Fuel_Consumed: {
            if (role == Qt::UserRole || role == Qt::DisplayRole) {
                QPersistentModelIndex maneuver = item->data(TurnItem::Turn_Maneuver_Col).toPersistentModelIndex();
                int maneuver_fuel_cost = maneuver.sibling(maneuver.row(), ManeuverItem::Speed).data().toInt();

                int current_alt = item->data(TurnItem::Turn_Altitude_Col).toInt();
                int last_alt = idx.row() <= 0 ? starting_alt : index(idx.row() - 1, TurnItem::Turn_Altitude_Col).data(Qt::UserRole).toInt();
                int last_last_alt = idx.row() <= 1 ? starting_alt : index(idx.row() - 2, TurnItem::Turn_Altitude_Col).data(Qt::UserRole).toInt();

                // Same alt
                if (last_alt == current_alt) {
                    return maneuver_fuel_cost;
                }
                // Climbed
                else if (last_alt < current_alt) {
                    // Zoom climbing
                    if (last_last_alt >= last_alt+2) {
                        return maneuver_fuel_cost;
                    }

                    return maneuver_fuel_cost + 1;
                }
                // Dove
                else {
                    return maneuver_fuel_cost - 1;
                }
            }
        }
        case TurnItem::Turn_Fuel_Remaining_Col: {
            if (role == Qt::UserRole) {
                return item->data(idx.column());
            }
            else if (role == Qt::DisplayRole || role == Qt::ForegroundRole) {
                QPersistentModelIndex plane_idx = item->data(TurnItem::Turn_Maneuver_Col).toPersistentModelIndex().parent();
                int max_fuel = plane_idx.sibling(plane_idx.row(), PlaneItem::Fuel).data().toInt();
                int fuel_consumed = 0;
                for (int i=0; i<=idx.row(); ++i) {
                    fuel_consumed += index(i, TurnItem::Turn_Fuel_Consumed).data(Qt::UserRole).toInt();
                }
                if (role == Qt::DisplayRole) {
                    return QString("%1/%2").arg(max_fuel-fuel_consumed).arg(max_fuel);
                }
                else if (max_fuel * 0.25 > max_fuel-fuel_consumed) {
                    return QColor(Qt::red);
                }
                else if (max_fuel * 0.25 > max_fuel-fuel_consumed) {
                    return QColor(255,80,50);
                }
            }
            break;
        }
        default: {
            if (role == Qt::DisplayRole) {
                return item->data(idx.column());
            }
        }
        }
    }
    else if (item->getType() == BaseItem::Crew_Turn_Item_Type) {
        switch (idx.column()) {
        case TurnCrewItem::Turn_Crew_Col: {
            if (role == Qt::UserRole) {
                return item->data(idx.column());
            }
            else if (role == Qt::DisplayRole) {
                QPersistentModelIndex crew_idx = item->data(idx.column()).toPersistentModelIndex();
                return crew_idx.sibling(crew_idx.row(), CrewItem::Crew_Name).data().toString();
            }
            break;
        }
        default: {
            if (role == Qt::DisplayRole) {
                return item->data(idx.column());
            }
        }
        }
    }

    return QVariant();
}

bool TurnModel::setData(const QModelIndex &index, const QVariant &value, int role)
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

QVariant TurnModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || section < 0 || section >= TurnItem::Turn_Item_Col_Count) {
        return QAbstractItemModel::headerData(section, orientation, role);
    }

    switch (section) {
        case TurnItem::Turn_Number: return "";
        case TurnItem::Turn_Maneuver_Col: return "Maneuver/Crew";
        case TurnItem::Turn_Tolerance_Tag: return "Tolerance";
        case TurnItem::Turn_Altitude_Col: return "Altitude/Action";
        case TurnItem::Turn_Fuel_Consumed: return "Fuel Consumed";
        case TurnItem::Turn_Fuel_Remaining_Col: return "Fuel Remaining";
        default: return section;
    }
}

void TurnModel::addTurn(QPersistentModelIndex maneuver_idx, int alt, QList<QPair<QPersistentModelIndex, QString> > crew_actions)
{
    QModelIndex last_idx = lastIndex(TurnItem::Turn_Altitude_Col);
    int last_alt = last_idx.isValid() ? last_idx.data(Qt::UserRole).toInt() : starting_alt;

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    TurnItem* turn = new TurnItem(maneuver_idx, alt, root);
    root->addChild(turn);
    endInsertRows();

    if (alt > last_alt) {
        turn->setData(TurnItem::Turn_Tolerance_Tag, maneuver_idx.sibling(maneuver_idx.row(), ManeuverItem::Climb_Value).data());
    }
    else if (alt < last_alt) {
        turn->setData(TurnItem::Turn_Tolerance_Tag, maneuver_idx.sibling(maneuver_idx.row(), ManeuverItem::Dive_Value).data());
    }
    else {
        turn->setData(TurnItem::Turn_Tolerance_Tag, maneuver_idx.sibling(maneuver_idx.row(), ManeuverItem::Level_Value).data());
    }

    beginInsertRows(lastIndex(), 0, crew_actions.size()-1);
    for (auto pair : crew_actions) {
        turn->addChild(new TurnCrewItem(pair.first, pair.second, turn));
    }
    endInsertRows();
}

void TurnModel::clearModel()
{
    beginResetModel();
    delete root;
    root = new BaseItem(BaseItem::Base_Item_Type);
    endResetModel();
}

QModelIndex TurnModel::lastIndex(int column, QModelIndex parent) const
{
    if (rowCount(parent) == 0) {
        return QModelIndex();
    }
    return index(rowCount(parent)-1, column);
}

TurnFilterProxy::TurnFilterProxy(TurnModel *src_model, QObject *parent) : QSortFilterProxyModel(parent)
{
    setSourceModel(src_model);
}

bool TurnFilterProxy::filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const
{
    if (!source_parent.isValid() && (source_column == TurnItem::Turn_Tolerance_Tag || source_column == TurnItem::Turn_Fuel_Consumed)) {
        return false;
    }
    return QSortFilterProxyModel::filterAcceptsColumn(source_column, source_parent);
}
