#-------------------------------------------------
#
# Project created by QtCreator 2011-12-30T16:38:40
#
#-------------------------------------------------

QT       += core gui network widgets webview printsupport webenginewidgets

TARGET = Schoolar
TEMPLATE = app

RC_FILE = Schoolar.rc


SOURCES += \
    Base/formulaplotter.cpp \
    Base/tphrases.cpp \
    Base/tline.cpp \
    Base/tinterval.cpp \
#    Base/taxes.cpp \
    Base/polynom.cpp \
    Base/arithmetic.cpp \
    Problems/ttrigonometryproblems.cpp \
    Problems/trationalsumproblem.cpp \
    Problems/tproblem.cpp \
    Problems/tpowerproblems.cpp \
    Problems/ObjectsFactory.cpp \
#    Problems/GraphicsProblems.cpp \
    algebra_operations.cpp \
    twaitform.cpp \
    ttasktypeselector.cpp \
    taboutdialog.cpp \
    teuladialog.cpp \
    tfreeaboutdialog.cpp \
    toptionswindow.cpp \
    qformulaeditor.cpp \
    main.cpp \
    mainwindow.cpp \
    Problems/tsystemofequations.cpp \
    mathomatic.cpp \
    tselectslausize.cpp \
    Base/symbols.cpp \
    Problems/tderivativeproblem.cpp \
    Problems/tmaclaurinseriesproblem.cpp \
    Problems/TPolynomIntegralproblem.cpp \
    Problems/TPolynomProblems.cpp \
    Base/TNumericMask.cpp \
    Base/THTMLWriter.cpp \
    tsolutionbrowser.cpp \
    tglobeview.cpp \
    Problems/RandomFunction.cpp \
    tbuttonssheetview.cpp \
    ttestfonts.cpp

HEADERS  += \
    Base/formulaplotter.h \
    Base/tphrases.h \
    Base/tline.h \
    Base/tinterval.h \
#    Base/taxes.h \
    Base/polynom.h \
    Base/arithmetic.h \
    Problems/ttrigonometryproblems.h \
    Problems/trationalsumproblem.h \
    Problems/tproblem.h \
    Problems/tpowerproblems.h \
    Problems/ObjectsFactory.h \
#    Problems/GraphicsProblems.h \
    twaitform.h \
    ttasktypeselector.h \
    taboutdialog.h \
    teuladialog.h \
    tfreeaboutdialog.h \
    toptionswindow.h \
    qformulaeditor.h \
    mainwindow.h \
    algebra_operations.h \
    Problems/tsystemofequations.h \
    mathomatic.h \
    tselectslausize.h \
    Base/symbols.h \
    Problems/tderivativeproblem.h \
    Problems/tmaclaurinseriesproblem.h \
    Problems/TPolynomProblems.h \
    Problems/TPolynomIntegralproblem.h \
    Base/TNumericMask.h \
    Base/THTMLWriter.h \
    tsolutionbrowser.h \
    tglobeview.h \
    Problems/RandomFunction.h \
    tbuttonssheetview.h \
    ttestfonts.h

FORMS    += \
    twaitform.ui \
    ttasktypeselector.ui \
    taboutdialog.ui \
    teuladialog.ui \
    tfreeaboutdialog.ui \
    toptionswindow.ui \
    mainwindow.ui \
    tselectslausize.ui \
    ttestfonts.ui

LIBS += -L../../mathomatic/mathomatic/lib -lmathomatic

INCLUDEPATH += mathomatic

RESOURCES += \
    Schoolar.qrc

OTHER_FILES += \
    style.css

QMAKE_CXXFLAGS += -O3

CONFIG += c++20
