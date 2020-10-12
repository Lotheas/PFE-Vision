QT       += core gui

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
QT += widgets
QT += gui

SOURCES += \
    camera_support.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h \
    camera_support.h \

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


# BASLER

INCLUDEPATH += $$PWD/pylon_includes

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/x64/ -lGCBase_MD_VC141_v3_1_Basler_pylon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/x64/ -lGCBase_MD_VC141_v3_1_Basler_pylon
else:unix: LIBS += -L$$PWD/lib/x64/ -lGCBase_MD_VC141_v3_1_Basler_pylon

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/x64/ -lGenApi_MD_VC141_v3_1_Basler_pylon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/x64/ -lGenApi_MD_VC141_v3_1_Basler_pylon
else:unix: LIBS += -L$$PWD/lib/x64/ -lGenApi_MD_VC141_v3_1_Basler_pylon

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/x64/ -lPylonBase_v6_1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/x64/ -lPylonBase_v6_1
else:unix: LIBS += -L$$PWD/lib/x64/ -lPylonBase_v6_1

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/x64/ -lPylonC
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/x64/ -lPylonC
else:unix: LIBS += -L$$PWD/lib/x64/ -lPylonC

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/x64/ -lPylonGUI_v6_1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/x64/ -lPylonGUI_v6_1
else:unix: LIBS += -L$$PWD/lib/x64/ -lPylonGUI_v6_1

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/x64/ -lPylonUtility_v6_1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/x64/ -lPylonUtility_v6_1
else:unix: LIBS += -L$$PWD/lib/x64/ -lPylonUtility_v6_1

INCLUDEPATH += $$PWD/lib/x64
DEPENDPATH += $$PWD/lib/x64

# OpenCV (les chemins sont absolus, il faut donc les retrouver sur son pc)

INCLUDEPATH += D:\OCV\Install\opencv\include

CONFIG(debug, debug|release) {
    LIBS += "D:/OCV/Build/opencv/lib/Debug/opencv_world450d.lib"
    # LIBS += "D:/OCV/Build/opencv/bin/Debug/opencv_world450d.dll"
}
else {
    LIBS += "D:/OCV/Build/opencv/lib/Release/opencv_world450.lib"
    # LIBS += "D:/OCV/Build/opencv/bin/Release/opencv_world450.dll"
}
