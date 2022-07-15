#include "CrewEditorTab.h"
#include "ui_CrewEditorTab.h"

CrewEditorTab::CrewEditorTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CrewEditorTab)
{
    ui->setupUi(this);
}

CrewEditorTab::~CrewEditorTab()
{
    delete ui;
}
