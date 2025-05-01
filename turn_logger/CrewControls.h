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
    enum WoundValues {
        None = 0,
        Light,
        Severe,
        Dead
    };

public:
    explicit CrewControls(const QPersistentModelIndex &crew_idx, QSharedPointer<FilterProxy> crew_proxy,
                          QSharedPointer<FilterProxy> maneuver_proxy, QSharedPointer<FilterProxy> turn_proxy, QWidget *parent = nullptr);
    ~CrewControls();

    // std::tuple<QPersistentModelIndex, int, QVariant> getChosenCrewAction();
    void populateTurnIdx(QPersistentModelIndex turn_crew_idx);
    void handleTurnEnd();

public slots:
    void updateBombState();
    void applyManeuverRestrictions(QPersistentModelIndex maneuver_idx);
    void refreshGunWidgets();

signals:
    void bombDropped();

private slots:
    void setSliderStylesheet(QString colour);
    void applyCVCalcs();

private:
    int calculateCV();
    QVariant getActionExtraData();
    void initConnections();

    Ui::CrewControls *ui;
    QPersistentModelIndex crew_idx; // When this is passed in, it is assumed to be set to column Crew_Name
    QPersistentModelIndex selected_maneuver;
    QSharedPointer<FilterProxy> crew_proxy = nullptr;
    QSharedPointer<FilterProxy> maneuver_proxy = nullptr;
    QSharedPointer<FilterProxy> turn_proxy = nullptr;
};

#endif // CREWCONTROLS_H
