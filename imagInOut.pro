#-------------------------------------------------
#
# Project created by QtCreator 2018-11-05T19:08:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = imagInOut
TEMPLATE = app
LIBS+=-lSDL2 -lSDL2_image -lSDL2_gfx
QT += multimedia multimediawidgets

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        message.cpp \
        preferences.cpp

HEADERS += \
        mainwindow.h \
        globals.h \
        message.h \
        preferences.h

FORMS += \
        mainwindow.ui \
        message.ui \
        preferences.ui

unix:!macx {
LIBS += -L /usr/lib/x86_64-linux-gnu/ -lSDL2 -ljsoncpp -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_gfx -lSDL2_ttf -lboost_system
INCLUDEPATH += /usr/include/SDL2 /usr/include
}

macx: {
LIBS += -L /usr/lib/ -L/opt/local/lib -L/usr/local/lib -F/Library/Frameworks -framework SDL2 -ljsoncpp -framework SDL2_image -framework SDL2_mixer -framework SDL2_ttf  -lboost_system
INCLUDEPATH += /opt/local/include
CONFIG += app_bundle  # to remove, do CONFIG -= app_bundle
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


