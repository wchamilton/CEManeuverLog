#ifndef PREGAMEPROMPT_H
#define PREGAMEPROMPT_H

#include <QDialog>
#include <QPersistentModelIndex>

namespace Ui {
class PreGamePrompt;
}

class PlaneFilterProxy;
class TurnModel;
class QLineEdit;
class QCheckBox;
class PreGamePrompt : public QDialog
{
    Q_OBJECT

    struct CrewInputWidgets {
        QLineEdit* crew_name_input = nullptr;
        QCheckBox* ignores_deflection = nullptr;
        QCheckBox* unrestricted_maneuvers = nullptr;
    };

public:
    explicit PreGamePrompt(PlaneFilterProxy* plane_model, QPersistentModelIndex plane_idx, TurnModel* turn_model, QWidget *parent = nullptr);
    ~PreGamePrompt() override;

public slots:
    void accept() override;

private:
    Ui::PreGamePrompt *ui;
    QList<QPair<QPersistentModelIndex, CrewInputWidgets>> inputs_by_index;
    PlaneFilterProxy* plane_model;
    QPersistentModelIndex plane_idx;
    TurnModel* turn_model;
};

#endif // PREGAMEPROMPT_H
