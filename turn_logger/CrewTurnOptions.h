#ifndef CREWTURNOPTIONS_H
#define CREWTURNOPTIONS_H

#include <QWidget>

namespace Ui {
class CrewTurnOptions;
}

class CrewTurnOptions : public QWidget
{
    Q_OBJECT

public:
    explicit CrewTurnOptions(QWidget *parent = nullptr);
    void addAction(QString actionName, QWidget *actionWidget);
    ~CrewTurnOptions();

private:
    Ui::CrewTurnOptions *ui;
};

#endif // CREWTURNOPTIONS_H
