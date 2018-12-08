QT += core
QT -= gui

TARGET = camara_find_armour
CONFIG += console
CONFIG -= app_bundle
CONFIG += C++11 #开启C++11新特性

TEMPLATE = app

SOURCES += main.cpp \
    v4l2_set.cpp \
    get_colors.cpp \
    find_armour.cpp \
    serialport.cpp \
    CRC_Check.cpp \
    anglesolve.cpp \
    ImgFactory.cpp

LIBS += -L/usr/local/lib \
     -lopencv_core \
     -lopencv_highgui \
     -lopencv_imgproc \
     -lopencv_videoio \
     -lopencv_imgcodecs \
     -lopencv_calib3d \
     -lopencv_features2d \
     -lopencv_video \
     -lopencv_flann \


HEADERS += \
    v4l2_set.h \
    get_colors.h \
    find_armour.h \
    serialport.h \
    CRC_Check.h \
    anglesolve.h \
    ImgFactory.h
