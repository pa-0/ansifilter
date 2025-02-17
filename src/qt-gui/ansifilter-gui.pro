TEMPLATE = app

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ansifilter-gui
DEPENDPATH += .
INCLUDEPATH += . ..
DEFINES += O2
QMAKE_CXXFLAGS += -std=c++17

# Input
HEADERS += mydialog.h
FORMS += ansifilter.ui
SOURCES += main.cpp mydialog.cpp
SOURCES += ../elementstyle.cpp ../plaintextgenerator.cpp ../codegenerator.cpp
SOURCES += ../platform_fs.cpp ../rtfgenerator.cpp ../htmlgenerator.cpp ../pangogenerator.cpp ../texgenerator.cpp ../latexgenerator.cpp ../bbcodegenerator.cpp ../svggenerator.cpp
SOURCES += ../stringtools.cpp ../stylecolour.cpp ../preformatter.cpp

RESOURCES += ansifilter.qrc
win32 {
    CONFIG += static
    DEFINES += _WIN32
    RC_FILE = icon.rc
    DESTDIR = ../../
    QMAKE_POST_LINK = F:\upx393w\upx.exe --best --force ../../ansifilter-gui.exe
}

macx-clang:ICON = $${PWD}/ansifilter.icns
macx:ICON = $${PWD}/ansifilter.icns

CONFIG += c++17
