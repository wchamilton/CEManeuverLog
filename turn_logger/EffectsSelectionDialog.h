#ifndef EFFECTSSELECTIONDIALOG_H
#define EFFECTSSELECTIONDIALOG_H

#include <QDialog>
#include <QPersistentModelIndex>

namespace Ui {
class EffectsSelectionDialog;
}

class PlaneModel;
class EffectsSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EffectsSelectionDialog(PlaneModel* plane_model, QPersistentModelIndex plane_idx, QWidget *parent = nullptr);
    ~EffectsSelectionDialog();

public slots:
    void accept() override;

private:
    Ui::EffectsSelectionDialog *ui;
    PlaneModel* plane_model;
    QPersistentModelIndex plane_idx;
};

#endif // EFFECTSSELECTIONDIALOG_H
