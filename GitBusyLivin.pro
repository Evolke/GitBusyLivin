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
    src/gbl/gbl_historymodel.cpp \
    src/ui/historyview.cpp \
    src/gbl/gbl_filemodel.cpp \
    src/ui/fileview.cpp \
    src/ui/clonedialog.cpp

HEADERS  += src/ui/mainwindow.h \
    src/gbl/gbl_repository.h \
    src/ui/qaboutdialog.h \
    src/gbl/gbl_historymodel.h \
    src/gbl/gbl_version.h \
    src/ui/historyview.h \
    src/gbl/gbl_filemodel.h \
    src/ui/fileview.h \
    src/ui/clonedialog.h

RESOURCES += \
    resources/gitbusylivin.qrc

win32:INCLUDEPATH += "D:/projects/qt/GitBusyLivin/libgit2/include"
unix:INCLUDEPATH += "/projects/qt/GitBusyLivin/libgit2/include"

win32:LIBS += D:/projects/qt/GitBusyLivin/libs/git2.lib \
    -lwinhttp \
    -lrpcrt4 \
    -lcrypt32 \
    -ladvapi32 \
    -lole32

macx:LIBS += /usr/lib/libiconv.dylib \
    /usr/lib/libSystem.dylib \
    /usr/lib/libcrypto.dylib \
    /usr/local/opt/openssl/lib/libssl.dylib \
    /usr/lib/libz.dylib \
    -framework CoreFoundation \
    -framework Security \
    /projects/qt/GitBusyLivin/libs/libgit2.a

RC_ICONS = resources/images/gitbusylivin.ico
ICON = resources/images/gitbusylivin.icns

DISTFILES += \
    resources/styles/zihuatanejo.qss \
    resources/styles/shawshank.qss


#CUSTOM CODE HERE
VERSION_MAJOR = 0
VERSION_MINOR = 0
VERSION_BUILD = 6

DEFINES += "VERSION_MAJOR=$$VERSION_MAJOR"\
       "VERSION_MINOR=$$VERSION_MINOR"\
       "VERSION_BUILD=$$VERSION_BUILD"

#Target version
VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}
