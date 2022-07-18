#ifndef PLANEPARTDAMAGETRACKER_H
#define PLANEPARTDAMAGETRACKER_H

#include <QGroupBox>
#include <QPersistentModelIndex>

namespace Ui {
class PlanePartDamageTracker;
}

class PlanePartDamageTracker : public QGroupBox
{
    Q_OBJECT

public:
    explicit PlanePartDamageTracker(QWidget *parent = nullptr);
    ~PlanePartDamageTracker();
    void setModelIndexes(QPersistentModelIndex hp, QPersistentModelIndex critical_hp);
    void clear();

private slots:
    void takeDamage();

private:
    Ui::PlanePartDamageTracker *ui;

    QPersistentModelIndex hp;
    QPersistentModelIndex critical_hp;
};

#endif // PLANEPARTDAMAGETRACKER_H
