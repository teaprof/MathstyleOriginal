cmake_minimum_required(VERSION 3.16)
project(Schoolar VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set up AUTOMOC and some sensible defaults for runtime execution
# When using Qt 6.3, you can replace the code block below with
# qt_standard_project_setup()
set(CMAKE_AUTOMOC ON)
include(GNUInstallDirs)
set(CMAKE_AUTOUIC ON)

#To install these packages use:
#sudo apt install qt6-base-dev qt6-webengine-dev-tools qt6-webview-dev
find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Network PrintSupport Widgets WebEngineWidgets) # QtWebview)

qt_add_executable(Schoolar WIN32 MACOSX_BUNDLE
    Base/THTMLWriter.cpp Base/THTMLWriter.h
    Base/TNumericMask.cpp Base/TNumericMask.h
    Base/arithmetic.cpp Base/arithmetic.h
    Base/polynom.cpp Base/polynom.h
    Base/symbols.cpp Base/symbols.h    
    #Base/taxes.cpp Base/taxes.h
    Base/tinterval.cpp Base/tinterval.h
    Base/tline.cpp Base/tline.h
    Base/formulaplotter.cpp Base/formulaplotter.h
    Base/tphrases.cpp Base/tphrases.h
    Base/PrettyPrinterTransformer.cpp Base/PrettyPrinterTransformer.h
    #Problems/GraphicsProblems.cpp Problems/GraphicsProblems.h
    Problems/ObjectsFactory.cpp Problems/ObjectsFactory.h
    Problems/RandomFunction.cpp Problems/RandomFunction.h
    #Problems/TPolynomIntegralproblem.cpp Problems/TPolynomIntegralproblem.h
    Problems/TPolynomProblems.cpp Problems/TPolynomProblems.h
    #Problems/tderivativeproblem.cpp Problems/tderivativeproblem.h
    #Problems/tmaclaurinseriesproblem.cpp Problems/tmaclaurinseriesproblem.h
    #Problems/tpowerproblems.cpp Problems/tpowerproblems.h
    Problems/tproblem.cpp Problems/tproblem.h
    #Problems/trationalsumproblem.cpp Problems/trationalsumproblem.h
    #Problems/tsystemofequations.cpp Problems/tsystemofequations.h
    #Problems/ttrigonometryproblems.cpp Problems/ttrigonometryproblems.h
    algebra_operations.cpp algebra_operations.h
    main.cpp
    mainwindow.cpp mainwindow.h mainwindow.ui
    mathomatic.cpp mathomatic.h
    qformulaeditor.cpp qformulaeditor.h
    taboutdialog.cpp taboutdialog.h taboutdialog.ui
    tbuttonssheetview.cpp tbuttonssheetview.h
    teuladialog.cpp teuladialog.h teuladialog.ui
    tfreeaboutdialog.cpp tfreeaboutdialog.h tfreeaboutdialog.ui
    tglobeview.cpp tglobeview.h
    toptionswindow.cpp toptionswindow.h toptionswindow.ui
    tselectslausize.cpp tselectslausize.h tselectslausize.ui
    tsolutionbrowser.cpp tsolutionbrowser.h
    ttasktypeselector.cpp ttasktypeselector.h ttasktypeselector.ui
    ttestfonts.cpp ttestfonts.h ttestfonts.ui
    twaitform.cpp twaitform.h twaitform.ui
)
target_include_directories(Schoolar PRIVATE
    mathomatic
)

target_link_libraries(Schoolar PRIVATE
    # Remove: L../../mathomatic/mathomatic/lib
    Qt::Core
    Qt::Gui
    Qt::Network
    Qt::PrintSupport
    Qt::WebEngineWidgets
    Qt::Widgets
    mathomatic
)

target_link_directories(Schoolar PRIVATE mathomatic/lib)

#target_compile_options(Schoolar
#    -O3
#)


# Resources:
set(Schoolar_resource_files
    "Pictures/Tasks/blue.png"
    "Pictures/Tasks/cyan.png"
    "Pictures/Tasks/derivative.png"
    "Pictures/Tasks/equation.png"
    "Pictures/Tasks/green.png"
    "Pictures/Tasks/inequality.png"
    "Pictures/Tasks/lightblue.png"
    "Pictures/Tasks/lightgreen.png"
    "Pictures/Tasks/lightpurple.png"
    "Pictures/Tasks/lightred.png"
    "Pictures/Tasks/purple.png"
    "Pictures/Tasks/red.png"
    "Pictures/Tasks/saved.png"
    "Pictures/Tasks/simplify.png"
    "Pictures/Tasks/systemsofequations.png"
    "Pictures/Tasks/systemsofinequalities.png"
    "Pictures/Tasks/yellow.png"
)

qt_add_resources(Schoolar "Schoolar"
    PREFIX
        "/Tasks"
    FILES
        ${Schoolar_resource_files}
)
set(Schoolar1_resource_files
    "Pictures/Commands/1998_low_cost_clock_256x256.png"
    "Pictures/Commands/Check_48x48.png"
    "Pictures/Commands/Delete_48x48.png"
    "Pictures/Commands/Favorites_48x48.png"
    "Pictures/Commands/Heart-icon.png"
    "Pictures/Commands/New_48x48.png"
    "Pictures/Commands/Open_48x48.png"
    "Pictures/Commands/Save_48x48.png"
    "Pictures/Commands/Settings_48x48.png"
    "Pictures/Commands/Test.png"
    "Pictures/Commands/Zoom In_48x48.png"
    "Pictures/Commands/Zoom Out_48x48.png"
    "Pictures/Commands/arrow_up_green_48.png"
    "Pictures/Commands/browser.png"
    "Pictures/Commands/coins.png"
    "Pictures/Commands/comment_48.png"
    "Pictures/Commands/cross_48.png"
    "Pictures/Commands/file_pdf.png"
    "Pictures/Commands/floppy_disk_48.png"
    "Pictures/Commands/print.png"
    "Pictures/tea/lamp_scaled.png"
)

qt_add_resources(Schoolar "Schoolar1"
    PREFIX
        "/Commands"
    FILES
        ${Schoolar1_resource_files}
)
set(Schoolar2_resource_files
    "Icon/favicon.png"
    "Pictures/Style/Help_32x32.png"
    "Pictures/Style/book_48.png"
    "Pictures/Style/bulb.png"
    "Pictures/Style/downarrow.png"
    "Pictures/Style/folder_48.png"
    "Pictures/Style/leftarrow.png"
    "Pictures/Style/moleskine_black.png"
    "Pictures/Style/rightarrow.png"
    "Pictures/Style/stripesforhoriz.png"
    "Pictures/Style/stripesforvertical.png"
    "Pictures/Style/uparrow.png"
)

qt_add_resources(Schoolar "Schoolar2"
    PREFIX
        "/Style"
    FILES
        ${Schoolar2_resource_files}
)
set(Schoolar3_resource_files
    "Pictures/Formulas/acos.png"
    "Pictures/Formulas/arrow_left_48.png"
    "Pictures/Formulas/arrow_left_green_48.png"
    "Pictures/Formulas/arrow_right_48.png"
    "Pictures/Formulas/arrow_right_green_48.png"
    "Pictures/Formulas/asin.png"
    "Pictures/Formulas/atg.png"
    "Pictures/Formulas/ch.png"
    "Pictures/Formulas/clear.png"
    "Pictures/Formulas/cos.png"
    "Pictures/Formulas/edit.png"
    "Pictures/Formulas/erase.png"
    "Pictures/Formulas/exp.png"
    "Pictures/Formulas/frac.png"
    "Pictures/Formulas/ln.png"
    "Pictures/Formulas/log.png"
    "Pictures/Formulas/minus.png"
    "Pictures/Formulas/plus.png"
    "Pictures/Formulas/power.png"
    "Pictures/Formulas/prod.png"
    "Pictures/Formulas/recycle.png"
    "Pictures/Formulas/root.bmp"
    "Pictures/Formulas/sh.png"
    "Pictures/Formulas/simplify.png"
    "Pictures/Formulas/sin.png"
    "Pictures/Formulas/solve.png"
    "Pictures/Formulas/sqrt.png"
    "Pictures/Formulas/tg.png"
    "Pictures/Formulas/x.png"
    "Pictures/acrobat_icon1.png"
)

qt_add_resources(Schoolar "Schoolar3"
    PREFIX
        "/formulas"
    FILES
        ${Schoolar3_resource_files}
)
set(Schoolar4_resource_files
    "Eula/EULA_english_translated.html"
    "Eula/EULA_russian.html"
)

qt_add_resources(Schoolar "Schoolar4"
    PREFIX
        "/EULA"
    FILES
        ${Schoolar4_resource_files}
)

install(TARGETS Schoolar
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Consider using qt_generate_deploy_app_script() for app deployment if
# the project can use Qt 6.3. In that case rerun qmake2cmake with
# --min-qt-version=6.3.

find_package(Threads REQUIRED)
link_libraries(Threads::Threads)