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

    enum ActionTaken {
        No_Action = 0,
        Shoot_Action,
        Reload_Action,
        Unjam_Action,
        Observe_Action,
        Drop_Payload_Action,
        Custom_Action
    };

public:
    explicit CrewControls(PlaneFilterProxy *model, QPersistentModelIndex crew_idx, TurnModel* turn_model, QWidget *parent = nullptr);
    ~CrewControls();
    QPair<QPersistentModelIndex, QString> getChosenCrewAction();
    void handleTurnEnd();

signals:
    void cv_component_changed();

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
