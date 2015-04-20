            #-------------------------------------------------
# File .pro created by H. Saadaoui 2015-01-10T
#-------------------------------------------------

TEMPLATE     = app

QT          += core gui sql printsupport webkitwidgets

CONFIG      += qt warn_off release include_dependpath

#DEFINES     += QT_NO_DEBUG_OUTPUT

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEPENDPATH  += ./src ./src/external ./etc \
               ./src/tab.search \
               ./src/tab.analysis \
               ./src/tab.results \
               ./src/tab.simulations \
               ./src/tab.database \
               ./src/tab.converter \
               ./src/dialog.stream

INCLUDEPATH += $$DEPENDPATH

VPATH       += $$DEPENDPATH


LIBS        += -lmud -lMinuit2 -lgomp

SOURCES     += main.cpp \
               mainwindow.cpp \
               dialog_stream.cpp \
               tab_search.cpp mudfunctions.c \
               tab_analysis.cpp fitting_routine.cpp plugin_read.cpp read_data.cpp \
               bnmr.cpp ascii.cpp \
               tab_results.cpp  \
               tab_simulations.cpp vanVleck.cpp energygap.cpp \
               tab_database.cpp \
               tab_converter.cpp \
               plotting.cpp qcustomplot.cpp

HEADERS     += mainwindow.h constants.h \
               dialog_stream.h \
               tab_search.h \
               tab_analysis.h  fitting_routine.h plugin_read.h read_data.h parameters.h \
               bnmr.h ascii.h \
               tab_results.h \
               tab_simulations.h vanVleck.h energygap.h \
               tab_database.h \
               tab_converter.h \
               plotting.h  qcustomplot.h

FORMS       += mainwindow.ui \
               dialog_stream.ui \
               tab_search.ui \
               tab_analysis.ui \
               tab_results.ui \
               tab_simulations.ui \
               tab_converter.ui \
               tab_database.ui

BUILDDIR     = $$PWD/bin
OBJECTS_DIR  = $${BUILDDIR}
MOC_DIR      = $${BUILDDIR}
RCC_DIR      = $${BUILDDIR}
UI_DIR       = $${BUILDDIR}

RESOURCES   += ./etc/resources.qrc

TARGET       = ./bnmroffice

isEmpty(PREFIX) { PREFIX = /opt/bnmroffice }

exe.path     = $$PREFIX/bin
exe.files    = bnmroffice

doc.path     = $$PREFIX/doc
doc.files    = etc/doc/*

INSTALLS    +=  exe doc
