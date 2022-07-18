#ifndef PLANEMODEL_H
#define PLANEMODEL_H

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QDebug>
#include "PlaneItems.h"

class PlaneModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit PlaneModel(QObject *parent = nullptr);
    ~PlaneModel() override;

    // Basic functionality:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    Qt::ItemFlags flags(const QModelIndex &idx) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QModelIndex loadPlaneJSON(QJsonObject plane);
    QJsonObject dumpPlaneToJSON(const QModelIndex &index);
    void prepareTemplateModel();
    void clearModel();

private:
    BaseItem* root = nullptr;
};

class PlaneFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    PlaneFilterProxy(PlaneModel* src_model, QObject* parent = nullptr);
    void setTypeFilter(QList<BaseItem::ItemType> type);
    void expandFilter(BaseItem::ItemType type);

    // QSortFilterProxyModel interface
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

private:
    QList<BaseItem::ItemType> type_filters;
};

#endif // PLANEMODEL_H
