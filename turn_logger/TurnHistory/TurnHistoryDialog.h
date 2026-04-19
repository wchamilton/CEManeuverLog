#ifndef TURNHISTORYDIALOG_H
#define TURNHISTORYDIALOG_H

#include <QDialog>

namespace Ui {
class TurnHistoryDialog;
}

class GameModel;
class FilterProxy;
class TurnHistoryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TurnHistoryDialog(GameModel* game_model, QSharedPointer<FilterProxy> crew_proxy, QWidget *parent = nullptr);
    ~TurnHistoryDialog();

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void updateSelectedTurnReport(const QModelIndex &selected_turn);

private:
    Ui::TurnHistoryDialog *ui;
    GameModel* game_model;
    QMap<QString, int> crew_tabs;
};

#endif // TURNHISTORYDIALOG_H
