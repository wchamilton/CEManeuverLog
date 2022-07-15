#ifndef GUNCONTROLS_H
#define GUNCONTROLS_H

#include <QWidget>

namespace Ui {
class GunControls;
}

class GunControls : public QWidget
{
    Q_OBJECT

public:
    explicit GunControls(QWidget *parent = nullptr);
    ~GunControls();

private:
    Ui::GunControls *ui;
};

#endif // GUNCONTROLS_H
