//#include "turn_logger/MainWindow.h"
#include "EntryDialog.h"
#include "CEManeuvers.h"

#include <QApplication>
#include <QFile>
#include <QStyle>
//#include <QDesktopWidget>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile file("../../../CEManeuverLog/stylesheet.qss");
    file.open(QFile::ReadOnly);
    QString style_sheet = QLatin1String(file.readAll());
    a.setStyleSheet(style_sheet);

    QSettings::setDefaultFormat(QSettings::IniFormat);
    qApp->setOrganizationName("Name Pending");
    qApp->setApplicationName("CEManeuverLog");
    a.setWindowIcon(QIcon(GRAPHICS_LOCATION + "/plane_icon.png"));

    EntryDialog dlg;
    dlg.show();
    return a.exec();
}
