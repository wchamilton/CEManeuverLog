#ifndef CREWCONTROLS_H
#define CREWCONTROLS_H

#include <QWidget>

namespace Ui {
class CrewControls;
}

class PlaneFilterProxy;
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
    explicit CrewControls(PlaneFilterProxy *model, QPersistentModelIndex crew_idx, QWidget *parent = nullptr);
    ~CrewControls();

private slots:
    void setSliderStylesheet(QString colour);

private:
    void populateGunControls(QPersistentModelIndex gun_idx);

    Ui::CrewControls *ui;
};

#endif // CREWCONTROLS_H
