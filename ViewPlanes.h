#ifndef VIEWPLANES_H
#define VIEWPLANES_H

#include <QDialog>

namespace Ui {
class ViewPlanes;
}

class ViewPlanes : public QDialog
{
    Q_OBJECT

public:
    explicit ViewPlanes(QWidget *parent = nullptr);
    ~ViewPlanes();

private:
    Ui::ViewPlanes *ui;
};

#endif // VIEWPLANES_H
