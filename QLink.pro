QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    block.cpp \
    gamewindow.cpp \
    main.cpp \
    player.cpp \
    qlinkmap.cpp \
    startwindow.cpp \
    uiconfig.cpp \
    uimanager.cpp \
    utils.cpp

HEADERS += \
    block.h \
    gamewindow.h \
    includes.h \
    player.h \
    qlinkmap.h \
    startwindow.h \
    types.h \
    uiconfig.h \
    uimanager.h \
    utils.h

FORMS += \
    startwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
