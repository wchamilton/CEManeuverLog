#ifndef PLANEPARTDAMAGETRACKER_H
#define PLANEPARTDAMAGETRACKER_H

#include <QGroupBox>

namespace Ui {
class PlanePartDamageTracker;
}

class PlanePartDamageTracker : public QGroupBox
{
    Q_OBJECT

public:
    explicit PlanePartDamageTracker(QWidget *parent = nullptr);
    ~PlanePartDamageTracker();

private:
    Ui::PlanePartDamageTracker *ui;
};

#endif // PLANEPARTDAMAGETRACKER_H
