#ifndef TURNMODEL_H
#define TURNMODEL_H

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include "TurnItems.h"

class TurnModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit TurnModel(QObject *parent = nullptr);

    // Basic functionality:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    int getStartingSpeed() { return starting_speed; }
    int getStartingAlt() { return starting_alt; }
    void setStartingValues(int starting_alt, int starting_speed) {
        this->starting_alt = starting_alt;
        this->starting_speed = starting_speed;
    }

    void addTurn(QPersistentModelIndex maneuver_idx, int alt, QList<std::tuple<QPersistentModelIndex, int, QVariant> > crew_actions);
    void clearModel();
    QModelIndex lastIndex(int column = 0, QModelIndex parent = QModelIndex()) const;
    QModelIndex lastTurnPilotIndex(int column = 0);

private:
    BaseItem* root = nullptr;
    int starting_speed = 0;
    int starting_alt = 0;
};

class TurnFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    TurnFilterProxy(TurnModel* src_model, QObject* parent = nullptr);

protected:
    bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const override;
};

#endif // TURNMODEL_H
