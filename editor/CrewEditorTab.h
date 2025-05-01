#ifndef CREWEDITORTAB_H
#define CREWEDITORTAB_H

#include <QWidget>

namespace Ui {
class CrewEditorTab;
}

class FilterProxy;
class CrewEditorTab : public QWidget
{
    Q_OBJECT

public:
    explicit CrewEditorTab(QWidget *parent = nullptr);
    ~CrewEditorTab();
    void populateCrewItem(FilterProxy *model, QPersistentModelIndex crew_idx);
    void populateFromModel(FilterProxy* model, QPersistentModelIndex crew_idx);

private:
    Ui::CrewEditorTab *ui;
};

#endif // CREWEDITORTAB_H
