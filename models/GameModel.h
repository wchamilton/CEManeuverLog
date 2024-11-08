#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include <QAbstractItemModel>
#include "GameModelItems.h"

class GameModel : public QAbstractItemModel
{
    Q_OBJECT

    enum RootItems {
        Maneuvers = 0,
        Chits,
        Planes,
        Crew,
        Turns
    };

public:
    explicit GameModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    BaseItem* root = nullptr;
};

#endif // GAMEMODEL_H
