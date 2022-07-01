#ifndef CREWCONTROLS_H
#define CREWCONTROLS_H

#include <QWidget>

namespace Ui {
class CrewControls;
}

class CrewControls : public QWidget
{
    Q_OBJECT

public:
    explicit CrewControls(QWidget *parent = nullptr);
    ~CrewControls();

private:
    Ui::CrewControls *ui;
};

#endif // CREWCONTROLS_H
