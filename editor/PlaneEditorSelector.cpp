#include "PlaneEditorSelector.h"
#include "ui_PlaneEditorSelector.h"

#include "models/GameModelItems.h"

PlaneEditorSelector::PlaneEditorSelector(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PlaneEditorSelector)
{
    ui->setupUi(this);

    connect(ui->treeWidget, &QTreeWidget::currentItemChanged, this, [=](QTreeWidgetItem *current, QTreeWidgetItem *prev){
        if (current == prev) {
            return;
        }
        if (current == ui->treeWidget->topLevelItem(0) || current == ui->treeWidget->topLevelItem(1)) {
            ui->edit_selected_btn->setEnabled(false);
            return;
        }
        ui->edit_selected_btn->setEnabled(true);
    });
}

void PlaneEditorSelector::setModel(QAbstractItemModel *game_model, QModelIndex planes_root)
{
    for (int i=0; i<game_model->rowCount(planes_root); ++i) {
        QPersistentModelIndex plane_idx = game_model->index(i, PlaneItem::Plane_Name, planes_root);
        QTreeWidgetItem* plane_tree_item = new QTreeWidgetItem();
        plane_tree_item->setData(0, Qt::DisplayRole, plane_idx.data().toString());
        plane_tree_item->setData(0, Qt::UserRole, plane_idx);

        switch(game_model->index(plane_idx.row(), PlaneItem::Plane_Era, planes_root).data().toInt()) {
        case PlaneItem::Era_Early_War: ui->treeWidget->topLevelItem(0)->addChild(plane_tree_item); break;
        case PlaneItem::Era_Late_War:  ui->treeWidget->topLevelItem(1)->addChild(plane_tree_item); break;
        default: qWarning() << QString("Plane %1 was loaded with unknown era").arg(plane_idx.data().toString());
        }
    }
    ui->treeWidget->expandAll();
    ui->treeWidget->resizeColumnToContents(0);
}

QPersistentModelIndex PlaneEditorSelector::getSelectedPlane() const
{
    if (ui->treeWidget->selectedItems().isEmpty()){
        return QPersistentModelIndex();
    }
    return ui->treeWidget->selectedItems().first()->data(0, Qt::UserRole).toPersistentModelIndex();
}

PlaneEditorSelector::~PlaneEditorSelector()
{
    delete ui;
}
