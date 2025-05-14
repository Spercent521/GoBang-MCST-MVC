QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    boardwindow.cpp \
    circleButton.cpp \
    controller.cpp \
    main.cpp \
    mainwindow.cpp \
    mcts.cpp

HEADERS += \
    boardwindow.h \
    circleButton.h \
    controller.h \
    mainwindow.h \
    mcts.h

FORMS += \
    boardwindow.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    res/gobang_SZL.png

RESOURCES += \
    resources.qrc

CONFIG += resources_big

win32:CONFIG += console
