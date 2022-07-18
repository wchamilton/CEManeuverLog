#ifndef CREWEDITORTAB_H
#define CREWEDITORTAB_H

#include <QWidget>

namespace Ui {
class CrewEditorTab;
}

class CrewItem;
class CrewEditorTab : public QWidget
{
    Q_OBJECT

public:
    explicit CrewEditorTab(QWidget *parent = nullptr);
    ~CrewEditorTab();
    void populateCrewItem(CrewItem* crew);
    void populateFromJSON(QJsonObject crew);

private:
    Ui::CrewEditorTab *ui;
};

#endif // CREWEDITORTAB_H
