#ifndef PREGAMEPROMPT_H
#define PREGAMEPROMPT_H

#include <QDialog>
#include <QPersistentModelIndex>

namespace Ui {
class PreGamePrompt;
}

class PlaneFilterProxy;
class TurnModel;
class PreGamePrompt : public QDialog
{
    Q_OBJECT

public:
    explicit PreGamePrompt(PlaneFilterProxy* plane_model, QPersistentModelIndex plane_idx, TurnModel* turn_model, QWidget *parent = nullptr);
    ~PreGamePrompt() override;

public slots:
    void accept() override;

private:
    Ui::PreGamePrompt *ui;

    PlaneFilterProxy* plane_model;
    QPersistentModelIndex plane_idx;
    TurnModel* turn_model;
};

#endif // PREGAMEPROMPT_H
