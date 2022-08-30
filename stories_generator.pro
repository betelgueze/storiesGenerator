#-------------------------------------------------
#
# Project created by QtCreator 2022-08-07TXXXXXXXX
#
#-------------------------------------------------

QT       += core gui widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = stories_generator
TEMPLATE = app


SOURCES += main.cpp\
        codeEditor.cpp \
        interpretationslist.cpp \
        mainwindow.cpp

HEADERS  += mainwindow.h \
    codeEditor.h \
    interpretationslist.h \
    lineNumberArea.h \
    lineNumberAreaX.h \
    stories_generator.hpp

FORMS    += mainwindow.ui
