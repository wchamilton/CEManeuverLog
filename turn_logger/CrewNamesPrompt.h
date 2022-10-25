#ifndef CREWNAMESPROMPT_H
#define CREWNAMESPROMPT_H

#include <QDialog>
#include <QPersistentModelIndex>

namespace Ui {
class CrewNamesPrompt;
}

class PlaneFilterProxy;
class CrewNamesPrompt : public QDialog
{
    Q_OBJECT

public:
    explicit CrewNamesPrompt(PlaneFilterProxy* model, QPersistentModelIndex plane_idx, QWidget *parent = nullptr);
    ~CrewNamesPrompt() override;

public slots:
    void accept() override;

private:
    Ui::CrewNamesPrompt *ui;

    PlaneFilterProxy* model;
    QPersistentModelIndex plane_idx;
};

#endif // CREWNAMESPROMPT_H
