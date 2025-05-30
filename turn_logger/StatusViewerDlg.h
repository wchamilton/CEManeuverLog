#ifndef STATUSVIEWERDLG_H
#define STATUSVIEWERDLG_H

#include <QDialog>
#include <QPersistentModelIndex>

#include "models/GameModelItems.h"

namespace Ui {
class StatusViewerDlg;
}

class FilterProxy;
class StatusViewerDlg : public QDialog
{
    Q_OBJECT

public:
    explicit StatusViewerDlg(const QSharedPointer<FilterProxy> &crew_proxy, const QPersistentModelIndex &plane_idx, QWidget *parent = nullptr);
    ~StatusViewerDlg();

public slots:
    void accept();

private slots:
    void addEffect();
    void resetEffects();

private:
    void addStatusEffectToTable(const PlaneItem::Effect &effect);
    void removeStatusEffectFromTable(const PlaneItem::Effect &effect);

    Ui::StatusViewerDlg *ui;
    QSharedPointer<FilterProxy> crew_proxy;
    QPersistentModelIndex plane_idx;
    QList<PlaneItem::Effect> effects;
    QModelIndexList guns_to_destroy;
    int fuel_lost = 0;
};

#endif // STATUSVIEWERDLG_H
