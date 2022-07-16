#ifndef CREWCONTROLS_H
#define CREWCONTROLS_H

#include <QWidget>

namespace Ui {
class CrewControls;
}

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
    explicit CrewControls(QWidget *parent = nullptr);
    ~CrewControls();

private slots:
    void setSliderStylesheet(int value);

private:
    Ui::CrewControls *ui;
};

#endif // CREWCONTROLS_H
