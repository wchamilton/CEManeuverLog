QT += core widgets gui svgwidgets

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
    EntryDialog.cpp \
    editor/PlaneEditorSelector.cpp \
    graphics/AltCtrlScene.cpp \
    graphics/FiringArc.cpp \
    graphics/FiringArcScene.cpp \
    graphics/ManeuverModifiers.cpp \
    graphics/SelectionBox.cpp \
    main.cpp \
    CEManeuvers.cpp \
    models/GameModel.cpp \
    models/GameModelItems.cpp \
    turn_logger/GunRotationControl.cpp \
    turn_logger/CrewControls.cpp \
    turn_logger/PlanePartDamageTracker.cpp \
    graphics/HexTile.cpp \
    graphics/ManeuverGraphic.cpp \
    graphics/ManeuverScene.cpp \
    editor/PlaneEditor.cpp \
    editor/CrewEditorTab.cpp \
    editor/GunEditorTab.cpp \
    turn_logger/PlaneSelectionDialog.cpp \
    turn_logger/StatusEffectsInterface.cpp \
    turn_logger/TurnHistory/TurnHistoryCrewTab.cpp \
    turn_logger/TurnHistory/TurnHistoryDialog.cpp \
    turn_logger/TurnHistory/TurnHistoryGunTab.cpp \
    turn_logger/TurnTrackerDialog.cpp

HEADERS += \
    CEManeuvers.h \
    EntryDialog.h \
    editor/PlaneEditorSelector.h \
    graphics/AltCtrlScene.h \
    graphics/FiringArc.h \
    graphics/FiringArcScene.h \
    graphics/ManeuverModifiers.h \
    graphics/SelectionBox.h \
    models/GameModel.h \
    models/GameModelItems.h \
    turn_logger/GunRotationControl.h \
    turn_logger/CrewControls.h \
    turn_logger/PlanePartDamageTracker.h \
    graphics/HexTile.h \
    graphics/ManeuverGraphic.h \
    graphics/ManeuverScene.h \
    editor/PlaneEditor.h \
    editor/CrewEditorTab.h \
    editor/GunEditorTab.h \
    turn_logger/PlaneSelectionDialog.h \
    turn_logger/StatusEffectsInterface.h \
    turn_logger/TurnHistory/TurnHistoryCrewTab.h \
    turn_logger/TurnHistory/TurnHistoryDialog.h \
    turn_logger/TurnHistory/TurnHistoryGunTab.h \
    turn_logger/TurnTrackerDialog.h

FORMS += \
    EntryDialog.ui \
    editor/PlaneEditorSelector.ui \
    turn_logger/GunRotationControl.ui \
    turn_logger/CrewControls.ui \
    turn_logger/PlanePartDamageTracker.ui \
    editor/PlaneEditor.ui \
    editor/CrewEditorTab.ui \
    editor/GunEditorTab.ui \
    turn_logger/PlaneSelectionDialog.ui \
    turn_logger/StatusEffectsInterface.ui \
    turn_logger/TurnHistory/TurnHistoryCrewTab.ui \
    turn_logger/TurnHistory/TurnHistoryDialog.ui \
    turn_logger/TurnHistory/TurnHistoryGunTab.ui \
    turn_logger/TurnTrackerDialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    graphics/Canvas_AircraftSheet.svg \
    stylesheet.qss
