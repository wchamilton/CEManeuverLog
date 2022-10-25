#ifndef CREWCONTROLS_H
#define CREWCONTROLS_H

#include <QWidget>
#include <QPersistentModelIndex>

namespace Ui {
class CrewControls;
}

class PlaneFilterProxy;
class QAbstractButton;
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
    explicit CrewControls(PlaneFilterProxy *model, QPersistentModelIndex crew_idx, QWidget *parent = nullptr);
    ~CrewControls();
    QString getChosenAction();
    void handleTurnEnd();

signals:
    void updateSelectedAction();

private slots:
    void setSliderStylesheet(QString colour);
    void refreshGunWidgets(int row);

private:
    Ui::CrewControls *ui;

    PlaneFilterProxy *model = nullptr;
    QPersistentModelIndex crew_idx;
};

#endif // CREWCONTROLS_H
