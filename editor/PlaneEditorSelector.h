#ifndef PLANEEDITORSELECTOR_H
#define PLANEEDITORSELECTOR_H

#include <QDialog>

namespace Ui {
class PlaneEditorSelector;
}

class QAbstractItemModel;
class PlaneEditorSelector : public QDialog
{
    Q_OBJECT

public:
    explicit PlaneEditorSelector(QWidget *parent = nullptr);
    void setModel(QAbstractItemModel* model, QModelIndex planes_root);
    QPersistentModelIndex getSelectedPlane() const;
    ~PlaneEditorSelector();

private:
    Ui::PlaneEditorSelector *ui;
};

#endif // PLANEEDITORSELECTOR_H
