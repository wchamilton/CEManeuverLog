#ifndef PLANESELECTIONDIALOG_H
#define PLANESELECTIONDIALOG_H

#include <QDialog>

namespace Ui {
class PlaneSelectionDialog;
}

class GameModel;
class FilterProxy;
class QTreeWidgetItem;
class QGraphicsScene;
class QGridLayout;
class PlaneSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlaneSelectionDialog(GameModel* game_model, QWidget *parent = nullptr);
    ~PlaneSelectionDialog();

public slots:
    void accept();

private slots:
    void planeSelected(QTreeWidgetItem* current, QTreeWidgetItem* prev);
    void checkIfStartReady();

private:
    void prepareCrewRows(QGridLayout *layout, QPersistentModelIndex crew_idx);

    Ui::PlaneSelectionDialog *ui;
    GameModel* game_model = nullptr;
    QSharedPointer<FilterProxy> maneuver_proxy;
    QSharedPointer<FilterProxy> crew_proxy;
    QMap<QString, QGraphicsScene*> scene_map;
    QMap<int, QWidget*> crew_widget_map;
};

#endif // PLANESELECTIONDIALOG_H
