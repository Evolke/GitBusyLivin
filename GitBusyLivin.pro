#-------------------------------------------------
#
# Project created by QtCreator 2016-11-22T17:56:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GitBusyLivin
TEMPLATE = app


SOURCES += src/main.cpp\
        src/ui/mainwindow.cpp \
    src/gbl/gbl_repository.cpp \
    src/ui/qaboutdialog.cpp \
    src/gbl/gbl_historymodel.cpp

HEADERS  += src/ui/mainwindow.h \
    src/gbl/gbl_repository.h \
    src/ui/qaboutdialog.h \
    src/gbl/gbl_historymodel.h

RESOURCES += \
    resources/gitbusylivin.qrc

win32:INCLUDEPATH += "D:/projects/qt/GitBusyLivin/libgit2/include"
unix:INCLUDEPATH += "/projects/qt/GitBusyLivin/libgit2/include"

win32:LIBS += D:/projects/qt/GitBusyLivin/libs/git2.lib
unix:LIBS += /projects/qt/GitBusyLivin/libs/libgit2.dylib

RC_ICONS = resources/images/gitbusylivin.ico
ICON = resources/images/gitbusylivin.icns
