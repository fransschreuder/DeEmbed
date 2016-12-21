#-------------------------------------------------
#
# Project created by QtCreator 2016-12-13T11:45:45
#
#-------------------------------------------------

QT       += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DeEmbed
TEMPLATE = app


SOURCES += main.cpp\
        deembed.cpp \
    touchstone-file.cpp \
    chart.cpp \
    vna-math.cpp \
    chart-settings.cpp \
    qcolorpicker.cpp \
    cal-dialog.cpp \
    cal-spar.cpp \
    calstdsdialog.cpp \
    qsciencespinbox.cpp

HEADERS  += deembed.h \
    touchstone-file.h \
    chart.h \
    typedefs.h \
    vna-math.h \
    spline.h \
    chart-settings.h \
    qcolorpicker.h \
    cal-dialog.h \
    cal-spar.h \
    calstdsdialog.h \
    qsciencespinbox.h \
    version.h

FORMS    += mainwindow.ui \
    chart-settings.ui \
    cal-dialog.ui \
    calstdsdialog.ui

RESOURCES += \
    icons.qrc
