#ifndef PLANEEDITOR_H
#define PLANEEDITOR_H

#include <QDialog>

namespace Ui {
class PlaneEditor;
}

class PlaneModel;
class PlaneFilterProxy;
class ManeuverScene;
class QDataWidgetMapper;
class PlaneEditor : public QDialog
{
    Q_OBJECT

public:
    explicit PlaneEditor(QWidget *parent = nullptr);
    PlaneEditor(const QJsonObject &plane_object, QWidget *parent = nullptr);
    ~PlaneEditor();

private slots:
    void updatePreview(int row);
    void exportJSON();

private:
    void initWidgets();
    void setManeuverData(int column, QVariant data);
    void addManeuverToSchedule(QPersistentModelIndex idx);
    void removeManeuverFromSchedule(QPersistentModelIndex idx);

    Ui::PlaneEditor *ui;
    PlaneModel* plane_model = nullptr;
    PlaneFilterProxy* maneuver_proxy_model = nullptr;
    ManeuverScene* maneuver_preview_scene = nullptr;
    ManeuverScene* maneuver_schedule_scene = nullptr;
};

#endif // PLANEEDITOR_H
