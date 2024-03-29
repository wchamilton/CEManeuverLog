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
    graphics/AltCtrlScene.cpp \
    graphics/FiringArc.cpp \
    graphics/FiringArcScene.cpp \
    graphics/ManeuverModifiers.cpp \
    graphics/SelectionBox.cpp \
    main.cpp \
    CEManeuvers.cpp \
    models/TurnItems.cpp \
    models/TurnModel.cpp \
    turn_logger/EffectsSelectionDialog.cpp \
    turn_logger/MainWindow.cpp \
    turn_logger/CrewControls.cpp \
    turn_logger/PlanePartDamageTracker.cpp \
    graphics/HexTile.cpp \
    graphics/ManeuverGraphic.cpp \
    graphics/ManeuverScene.cpp \
    models/PlaneItems.cpp \
    models/PlaneModel.cpp \
    editor/PlaneEditor.cpp \
    editor/CrewEditorTab.cpp \
    editor/GunEditorTab.cpp \
    turn_logger/PreGamePrompt.cpp

HEADERS += \
    CEManeuvers.h \
    graphics/AltCtrlScene.h \
    graphics/FiringArc.h \
    graphics/FiringArcScene.h \
    graphics/ManeuverModifiers.h \
    graphics/SelectionBox.h \
    models/TurnItems.h \
    models/TurnModel.h \
    turn_logger/EffectsSelectionDialog.h \
    turn_logger/MainWindow.h \
    turn_logger/CrewControls.h \
    turn_logger/PlanePartDamageTracker.h \
    graphics/HexTile.h \
    graphics/ManeuverGraphic.h \
    graphics/ManeuverScene.h \
    models/PlaneItems.h \
    models/PlaneModel.h \
    editor/PlaneEditor.h \
    editor/CrewEditorTab.h \
    editor/GunEditorTab.h \
    turn_logger/PreGamePrompt.h

FORMS += \
    turn_logger/EffectsSelectionDialog.ui \
    turn_logger/MainWindow.ui \
    turn_logger/CrewControls.ui \
    turn_logger/PlanePartDamageTracker.ui \
    editor/PlaneEditor.ui \
    editor/CrewEditorTab.ui \
    editor/GunEditorTab.ui \
    turn_logger/PreGamePrompt.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    graphics/Canvas_AircraftSheet.svg \
    stylesheet.qss
