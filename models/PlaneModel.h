#ifndef PLANEMODEL_H
#define PLANEMODEL_H

#include <QAbstractItemModel>
#include "PlaneItems.h"

class PlaneModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit PlaneModel(QJsonArray planes, QObject *parent = nullptr);
    ~PlaneModel() override;

    // Basic functionality:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

private:
    BaseItem* root = nullptr;
};

#endif // PLANEMODEL_H
