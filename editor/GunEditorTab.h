#ifndef GUNEDITORTAB_H
#define GUNEDITORTAB_H

#include <QWidget>

namespace Ui {
class GunEditorTab;
}

class FilterProxy;
class GunEditorTab : public QWidget
{
    Q_OBJECT

public:
    explicit GunEditorTab(QWidget *parent = nullptr);
    ~GunEditorTab();
    void populateGunItem(FilterProxy *model, QPersistentModelIndex gun_item);
    void populateFromModel(QPersistentModelIndex gun_idx);

private:
    Ui::GunEditorTab *ui;

};

#endif // GUNEDITORTAB_H
