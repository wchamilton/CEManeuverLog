#include "CrewControls.h"
#include "ui_CrewControls.h"

CrewControls::CrewControls(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CrewControls)
{
    ui->setupUi(this);
    connect(ui->add_wound_btn, &QPushButton::pressed, this, [&](){
        ui->wounds->setValue(ui->wounds->value()+1);
    });
    connect(ui->wounds, &QSlider::valueChanged, this, [&](int value){
        setSliderStylesheet(value);
    });
}

CrewControls::~CrewControls()
{
    delete ui;
}

void CrewControls::setSliderStylesheet(int value)
{
    QString colour;
    switch (value) {
        case WoundValues::None: colour = "blue"; break;
        case WoundValues::Light: colour = "yellow"; break;
        case WoundValues::Severe: colour = "red"; break;
        case WoundValues::Dead: colour = "gray"; break;
    }
    QString stylesheet;
    stylesheet = QString(
        "QSlider::groove:horizontal {"
        "border: 1px solid #bbb;"
        "background: white;"
        "height: 4px;"
        "border-radius: 4px;"
        "}"

        "QSlider::sub-page:horizontal {"
        "background-color: %1;"
        "border: 1px solid #777;"
        "height: 4px;"
        "border-radius: 4px;"
        "}"

        "QSlider::add-page:horizontal {"
        "background: #fff;"
        "border: 1px solid #777;"
        "height: 4px;"
        "border-radius: 4px;"
        "}"

        "QSlider::handle:horizontal {"
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #eee, stop:1 #ccc);"
        "border: 1px solid #777;"
        "width: 8px;"
        "margin-top: -6px;"
        "margin-bottom: -6px;"
        "border-radius: 4px;"
        "}"

        "QSlider::sub-page:horizontal:disabled {"
        "background: #bbb;"
        "border-color: #999;"
        "}"

        "QSlider::add-page:horizontal:disabled {"
        "background: #eee;"
        "border-color: #999;"
        "}"

        "QSlider::handle:horizontal:disabled {"
        "background: #eee;"
        "border: 1px solid #aaa;"
        "border-radius: 4px;"
        "}").arg(colour);
    ui->wounds->setStyleSheet(stylesheet);
}
