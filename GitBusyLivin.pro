#-------------------------------------------------
#
# Project created by QtCreator 2016-11-22T17:56:43
#
#-------------------------------------------------

QT       += core gui network svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GitBusyLivin
TEMPLATE = app

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.13

SOURCES += src/main.cpp\
        src/ui/mainwindow.cpp \
    src/gbl/gbl_repository.cpp \
    src/gbl/gbl_historymodel.cpp \
    src/ui/historyview.cpp \
    src/gbl/gbl_filemodel.cpp \
    src/ui/fileview.cpp \
    src/ui/clonedialog.cpp \
    src/ui/urlpixmap.cpp \
    src/gbl/gbl_storage.cpp \
    src/ui/prefsdialog.cpp \
    src/ui/stageddockview.cpp \
    src/ui/unstageddockview.cpp \
    src/ui/singleapplication.cpp \
    src/ui/aboutdialog.cpp \
    src/ui/toolbarcombo.cpp \
    src/gbl/gbl_string.cpp \
    src/ui/badgetoolbutton.cpp \
    src/ui/referencesview.cpp \
    src/gbl/gbl_refsmodel.cpp \
    src/ui/mdichild.cpp \
    src/gbl/gbl_threads.cpp \
    src/ui/commitdock.cpp \
    src/ui/contentview.cpp \
    src/ui/statusprogressbar.cpp \
    src/ui/scandialog.cpp \
    src/ui/scanmdichild.cpp

HEADERS  += src/ui/mainwindow.h \
    src/gbl/gbl_repository.h \
    src/gbl/gbl_historymodel.h \
    src/gbl/gbl_version.h \
    src/ui/historyview.h \
    src/gbl/gbl_filemodel.h \
    src/ui/fileview.h \
    src/ui/clonedialog.h \
    src/ui/urlpixmap.h \
    src/gbl/gbl_storage.h \
    src/ui/prefsdialog.h \
    src/ui/stageddockview.h \
    src/ui/unstageddockview.h \
    src/ui/singleapplication.h \
    src/ui/aboutdialog.h \
    src/ui/toolbarcombo.h \
    src/gbl/gbl_string.h \
    src/ui/badgetoolbutton.h \
    src/ui/referencesview.h \
    src/gbl/gbl_refsmodel.h \
    src/ui/mdichild.h \
    src/gbl/gbl_threads.h \
    src/ui/commitdock.h \
    src/ui/contentview.h \
    src/ui/statusprogressbar.h \
    src/ui/scandialog.h \
    src/ui/scanmdichild.h

RESOURCES += \
    resources/gitbusylivin.qrc

INCLUDEPATH += $$PWD/libgit2/include

win32:LIBS += -lwinhttp \
    -lrpcrt4 \
    -lcrypt32 \
    -ladvapi32 \
    -lole32

win32 {
    debug {
        LIBS += $$PWD/libs/debug/git2.lib
    }
    release {
        LIBS += $$PWD/libs/release/git2.lib
    }
}

macx:LIBS += /usr/lib/libiconv.dylib \
    /usr/lib/libSystem.dylib \
    /usr/lib/libcrypto.dylib \
    /usr/lib/libz.dylib \
    -framework CoreFoundation \
    -framework Security \

macx {
    debug {
        LIBS += $$PWD/libs/debug/libgit2.a
    }

    release {
        LIBS += $$PWD/libs/release/libgit2.a
    }
}

RC_ICONS = resources/images/gitbusylivin.ico
ICON = resources/images/gitbusylivin.icns

DISTFILES += \
    resources/styles/zihuatanejo.qss \
    resources/styles/shawshank.qss \
    resources/content/about.html


#CUSTOM CODE HERE
VERSION_MAJOR = 0
VERSION_MINOR = 0
VERSION_BUILD = 19

DEFINES += "VERSION_MAJOR=$$VERSION_MAJOR"\
       "VERSION_MINOR=$$VERSION_MINOR"\
       "VERSION_BUILD=$$VERSION_BUILD"

#Target version
VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}
