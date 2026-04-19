#ifndef TURNHISTORYGUNTAB_H
#define TURNHISTORYGUNTAB_H

#include <QWidget>

namespace Ui {
class TurnHistoryGunTab;
}

class FiringArcScene;
class TurnHistoryGunTab : public QWidget
{
    Q_OBJECT

public:
    explicit TurnHistoryGunTab(QPersistentModelIndex gun_idx, QWidget *parent = nullptr);
    ~TurnHistoryGunTab();

    void updateSelectedTurnReport(const QModelIndex &gun_turn_idx);

private:
    Ui::TurnHistoryGunTab *ui;
    FiringArcScene* fire_arc_scene;
};

#endif // TURNHISTORYGUNTAB_H
