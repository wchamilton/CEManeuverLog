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
    void setHPValues(const int &hp, const int &critical_hp);
    void clear();

private slots:
    void takeDamage();
    void updateBorderColour(int value);

private:
    Ui::PlanePartDamageTracker *ui;

    int critical_hp;
};

#endif // PLANEPARTDAMAGETRACKER_H
