#ifndef CREWCONTROLS_H
#define CREWCONTROLS_H

#include <QWidget>
#include <QPersistentModelIndex>

namespace Ui {
class CrewControls;
}

class FilterProxy;
class QAbstractButton;
class CrewControls : public QWidget
{
    Q_OBJECT
public:
    enum WoundValues {
        None = 0,
        Light,
        Severe,
        Dead
    };

    explicit CrewControls(const QPersistentModelIndex &crew_idx, QSharedPointer<FilterProxy> crew_proxy,
                          QSharedPointer<FilterProxy> maneuver_proxy, QWidget *parent = nullptr);
    ~CrewControls();
    void saveCrewData();
    QPersistentModelIndex getCrewIdx() const { return crew_idx; }

public slots:
    void updateBombState();
    void applyManeuverRestrictions(QPersistentModelIndex maneuver_idx);
    void refreshGunWidgets();

private slots:
    void setSliderStylesheet(QString colour);
    void applyCVCalcs();

private:
    int calculateCV();
    QVariant getActionExtraData(int action_taken);
    void initConnections();

    Ui::CrewControls *ui;
    QPersistentModelIndex crew_idx; // When this is passed in, it is assumed to be set to column Crew_Name
    QPersistentModelIndex selected_maneuver;
    QSharedPointer<FilterProxy> crew_proxy = nullptr;
    QSharedPointer<FilterProxy> maneuver_proxy = nullptr;
};

#endif // CREWCONTROLS_H
