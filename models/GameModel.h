#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

struct Maneuver;
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
    ~GameModel();

    // Basic functionality:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    // Returns the number of elements for row/column based on parent
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    // Get/Set data to the model
    QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &idx, const QVariant &value, int role = Qt::EditRole) override;

    // JSON IO
    void writeTurnsToJSON();
    void readTurnsFromJSON();

    // Creation functions
    QPersistentModelIndex addPlane();
    QPersistentModelIndex addManeuver(const Maneuver &m, const QModelIndex &parent_plane_idx);
    QPersistentModelIndex addCrew(const QModelIndex &parent_plane_idx);
    QPersistentModelIndex addGun(const QModelIndex &parent_crew_idx);
    QPersistentModelIndex addEffect(const QModelIndex &parent_plane_idx);
    QPersistentModelIndex addTurn();
    QPersistentModelIndex addTurnPlaneEffects(const QModelIndex &parent_turn);
    QPersistentModelIndex addTurnCrew(const QModelIndex &parent_turn);
    QPersistentModelIndex addTurnCrewGun(const QModelIndex &parent_turn_crew);

    // Removal functions
    void removeManeuver(const QModelIndex &idx);
    void removeEffect(const QModelIndex &idx);
    void removeChild(int row, const QModelIndex &parent);
    void removeChildren(const QModelIndex &parent);
    void resetToTurn(const QModelIndex &turn);

    // Convenience functions
    QModelIndex gameRootIdx() const;
    QModelIndex planesRootIdx() const;
    QModelIndex turnsRootIdx() const;
    QModelIndex currentTurn(int column = 0) const;

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
