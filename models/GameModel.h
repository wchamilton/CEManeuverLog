#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

class BaseItem;
class GameModel : public QAbstractItemModel
{
    Q_OBJECT
public:

    enum RootItems {
        Game_Root = 0,  ///< Misc Game metadata item
        Planes_Root,    ///< Root parent for all planes
        Turns_Root      ///< Root parent for all turns
    };

    explicit GameModel(QObject *parent = nullptr);

    // Basic functionality:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &idx, const QVariant &value, int role = Qt::EditRole) override;

    // Convenience functions
    QModelIndex currentTurn(int column = 0) const;
    void applyActiveEffect(int effect, int duration, QString desc);

private:
    BaseItem* root = nullptr;   ///< Base root for all items in the tree
};

class FilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    FilterProxy(QAbstractItemModel* src_model, QObject* parent = nullptr);
    void setTypeFilter(int type);
    void setTypeFilter(QList<int> types);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

private:
    QList<int> item_type_filter;
};

#endif // GAMEMODEL_H
