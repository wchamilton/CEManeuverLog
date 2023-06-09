#ifndef GUNEDITORTAB_H
#define GUNEDITORTAB_H

#include <QWidget>

namespace Ui {
class GunEditorTab;
}

class GunItem;
class GunEditorTab : public QWidget
{
    Q_OBJECT

public:
    explicit GunEditorTab(QWidget *parent = nullptr);
    ~GunEditorTab();
    void populateGunItem(GunItem* gun_item);
    void populateFromJSON(QJsonObject gun);

private:
    Ui::GunEditorTab *ui;

};

#endif // GUNEDITORTAB_H
