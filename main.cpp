#include "MainWindow.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile file("../CEManeuverLog/stylesheet.qss");
    file.open(QFile::ReadOnly);
    QString style_sheet = QLatin1String(file.readAll());
    a.setStyleSheet(style_sheet);

    MainWindow w;
    w.show();
    return a.exec();
}
