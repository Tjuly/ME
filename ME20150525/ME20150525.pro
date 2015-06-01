#-------------------------------------------------
#
# Project created by QtCreator 2015-05-25T15:35:13
#
#-------------------------------------------------

QT += core gui
QT += svg xml

contains(QT_CONFIG, opengl): QT += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ME20150525
TEMPLATE = app

INCLUDEPATH += D:\QT\OpenCV\opencvmingw\install\include\
INCLUDEPATH += D:\QT\OpenCV\opencvmingw\install\include\opencv\
INCLUDEPATH += D:\QT\OpenCV\opencvmingw\install\include\opencv2\

LIBS += D:\QT\OpenCV\opencvmingw\install\lib\libopencv_core231.dll.a
LIBS += D:\QT\OpenCV\opencvmingw\install\lib\libopencv_highgui231.dll.a
LIBS += D:\QT\OpenCV\opencvmingw\install\lib\libopencv_imgproc231.dll.a

SOURCES += main.cpp\
        mainwindow.cpp \
    Iconhelper/iconhelper.cpp \
    Painterwidget/painterwidget.cpp \
    cutdialog.cpp \
    SVGview/svgview.cpp \
    BasicPro.cpp \
    Draw3dPieChart.cpp \
    SVGlib/trans.cpp \
    SVGlib/trace.cpp \
    SVGlib/render.cpp \
    SVGlib/potracelib.cpp \
    SVGlib/lzw.cpp \
    SVGlib/jojo.cpp \
    SVGlib/greymap.cpp \
    SVGlib/flate.cpp \
    SVGlib/decompose.cpp \
    SVGlib/curve.cpp \
    SVGlib/bbox.cpp \
    SVGlib/backend_svg.cpp \
    SVGlib/backend_pgm.cpp \
    SVGlib/backend_eps.cpp \
    listitemform.cpp \
    imgshowform.cpp \
    Painterwidget/ShowWidget.cpp

HEADERS  += mainwindow.h \
    Iconhelper/iconhelper.h \
    Painterwidget/painterwidget.h \
    cutdialog.h \
    SVGview/svgview.h \
    BasicPro.h \
    Draw3dPieChart.h \
    SVGlib/trans.h \
    SVGlib/trace.h \
    SVGlib/render.h \
    SVGlib/progress.h \
    SVGlib/potracelib.h \
    SVGlib/main.h \
    SVGlib/lzw.h \
    SVGlib/lists.h \
    SVGlib/jojo.h \
    SVGlib/greymap.h \
    SVGlib/flate.h \
    SVGlib/decompose.h \
    SVGlib/curve.h \
    SVGlib/config.h \
    SVGlib/bitops.h \
    SVGlib/bitmap.h \
    SVGlib/bbox.h \
    SVGlib/backend_svg.h \
    SVGlib/backend_pgm.h \
    SVGlib/backend_eps.h \
    SVGlib/auxiliary.h \
    listitemform.h \
    imgshowform.h \
    Painterwidget/ShowWidget.h

FORMS    += mainwindow.ui \
    cutdialog.ui \
    listitemform.ui \
    imgshowform.ui

RESOURCES += \
    Src.qrc

OTHER_FILES += \
    CSS/blue.css
