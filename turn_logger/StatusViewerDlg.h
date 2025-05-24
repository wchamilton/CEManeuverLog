#ifndef STATUSVIEWERDLG_H
#define STATUSVIEWERDLG_H

#include <QDialog>
#include <QPersistentModelIndex>

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

private:
    void addStatusEffectToTable(const QVariant &effect);

    Ui::StatusViewerDlg *ui;
    QSharedPointer<FilterProxy> crew_proxy;
    QPersistentModelIndex plane_idx;
    QVariantList effects;
};

#endif // STATUSVIEWERDLG_H
