#ifndef CREWCONTROLS_H
#define CREWCONTROLS_H

#include <QWidget>
#include <QPersistentModelIndex>

namespace Ui {
class CrewControls;
}

class PlaneFilterProxy;
class QAbstractButton;
class TurnModel;
class CrewControls : public QWidget
{
    Q_OBJECT
    enum WoundValues {
        None = 0,
        Light,
        Severe,
        Dead
    };

public:
    explicit CrewControls(PlaneFilterProxy *model, QPersistentModelIndex crew_idx, TurnModel* turn_model, QWidget *parent = nullptr);
    ~CrewControls();
    QPair<QPersistentModelIndex, int> getChosenCrewAction();
    void handleTurnEnd();

public slots:
    void updateBombState();

signals:
    void bombDropped();

private slots:
    void setSliderStylesheet(QString colour);
    void refreshGunWidgets(int row);
    void applyCVCalcs();

private:
    int calculateCV();
    Ui::CrewControls *ui;

    PlaneFilterProxy *model = nullptr;
    QPersistentModelIndex crew_idx;
    TurnModel* turn_model = nullptr;
};

#endif // CREWCONTROLS_H
