#ifndef CREWEDITORTAB_H
#define CREWEDITORTAB_H

#include <QWidget>

namespace Ui {
class CrewEditorTab;
}

class CrewEditorTab : public QWidget
{
    Q_OBJECT

public:
    explicit CrewEditorTab(QWidget *parent = nullptr);
    ~CrewEditorTab();

private:
    Ui::CrewEditorTab *ui;
};

#endif // CREWEDITORTAB_H
