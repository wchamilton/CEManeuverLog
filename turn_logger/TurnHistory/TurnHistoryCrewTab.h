#ifndef TURNHISTORYCREWTAB_H
#define TURNHISTORYCREWTAB_H

#include <QWidget>
#include <QPersistentModelIndex>

namespace Ui {
class TurnHistoryCrewTab;
}

class FilterProxy;
class TurnHistoryCrewTab : public QWidget
{
    Q_OBJECT

public:
    explicit TurnHistoryCrewTab(QSharedPointer<FilterProxy> crew_proxy, QPersistentModelIndex crew_idx, QWidget *parent = nullptr);
    ~TurnHistoryCrewTab();

    void updateSelectedTurnReport(const QModelIndex &crew_turn_idx);

private:
    Ui::TurnHistoryCrewTab *ui;
    QSharedPointer<FilterProxy> crew_proxy;
    QPersistentModelIndex crew_idx;
    QMap<QString, int> gun_tabs;
};

#endif // TURNHISTORYCREWTAB_H
