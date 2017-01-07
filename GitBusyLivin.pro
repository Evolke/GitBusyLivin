#-------------------------------------------------
#
# Project created by QtCreator 2016-11-22T17:56:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GitBusyLivin
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    gbl_repository.cpp

HEADERS  += mainwindow.h \
    gbl_repository.h

RESOURCES += \
    gitbusylivin.qrc

win32:INCLUDEPATH += "D:/projects/qt/GitBusyLivin/libgit2/include"
unix:INCLUDEPATH += "/qt_projects/GitBusyLivin/libgit2/include"

win32:LIBS += D:/projects/qt/GitBusyLivin/libs/git2.lib
unix:LIBS += /qt_projects/GitBusyLivin/libs/libgit2.dylib
