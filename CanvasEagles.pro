QT       += core widgets gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CEManeuvers.cpp \
    CrewControls.cpp \
    CrewEditorTab.cpp \
    GunControls.cpp \
    PlaneEditor.cpp \
    PlanePartDamageTracker.cpp \
    graphics/HexTile.cpp \
    graphics/ManeuverGraphic.cpp \
    graphics/ManeuverScene.cpp \
    main.cpp \
    MainWindow.cpp \
    models/PlaneItems.cpp \
    models/PlaneModel.cpp

HEADERS += \
    CEManeuvers.h \
    CrewControls.h \
    CrewEditorTab.h \
    GunControls.h \
    MainWindow.h \
    PlaneEditor.h \
    PlanePartDamageTracker.h \
    ScrollEater.h \
    graphics/HexTile.h \
    graphics/ManeuverGraphic.h \
    graphics/ManeuverScene.h \
    models/PlaneItems.h \
    models/PlaneModel.h

FORMS += \
    CrewControls.ui \
    CrewEditorTab.ui \
    GunControls.ui \
    MainWindow.ui \
    PlaneEditor.ui \
    PlanePartDamageTracker.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    graphics/Canvas_AircraftSheet.svg
