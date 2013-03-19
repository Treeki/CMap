#-------------------------------------------------
#
# Project created by QtCreator 2012-04-04T00:16:15
#
#-------------------------------------------------

QT       += core gui widgets
QMAKE_CXXFLAGS += -std=c++11
TARGET = CMap
TEMPLATE = app


SOURCES += main.cpp\
    cpatchfile.cpp \
    cshapepool.cpp \
    cpatchcontext.cpp \
    ceditorwindow.cpp \
    cmapwidget.cpp \
    ceditablemap.cpp \
    ceditcommand.cpp \
    cmap.cpp \
    cedittoolbox.cpp \
    ctoolbase.cpp \
    cbrushtool.cpp \
    ccommandtoolbase.cpp \
    cmappoint.cpp \
    cshapepicker.cpp \
    cshapemodel.cpp \
    cobjectpicker.cpp \
    cundostack.cpp \
    utility.cpp \
    cshapeindexer.cpp \
    cshapeitemdelegate.cpp \
    clinetool.cpp \
    cdreamsettings.cpp \
    ceditorstatusbar.cpp \
    cresizedreamdialog.cpp \
    cevenspinbox.cpp \
    caboutdialog.cpp \
    cfilltool.cpp

HEADERS  += \
    cpatchfile.h \
    cshapepool.h \
    cpatchcontext.h \
    cmapvector.h \
    ceditorwindow.h \
    cmapwidget.h \
    ceditablemap.h \
    cmappoint.h \
    ceditcommand.h \
    cmap.h \
    cedittoolbox.h \
    ctoolbase.h \
    cbrushtool.h \
    ccommandtoolbase.h \
    cshapepicker.h \
    cshapemodel.h \
    cobjectpicker.h \
    cundostack.h \
    utility.h \
    cshapeindexer.h \
    cshapeitemdelegate.h \
    clinetool.h \
    cdreamsettings.h \
    ceditorstatusbar.h \
    cresizedreamdialog.h \
    cevenspinbox.h \
    caboutdialog.h \
    cfilltool.h

FORMS    += \
    cdreamsettings.ui \
    cresizedreamdialog.ui \
    caboutdialog.ui

RESOURCES += \
    resources.qrc
