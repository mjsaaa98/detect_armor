QT += core
QT -= gui

TARGET = camara_find_armour
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    video.cpp \
    v4l2_set.cpp \
    get_colors.cpp \
    find_armour.cpp \
    send_location.cpp \
    serialport.cpp

LIBS += -L/usr/local/lib \
     -lopencv_core \
     -lopencv_highgui \
     -lopencv_imgproc \
     -lopencv_videoio \
     -lopencv_imgcodecs \
     -lopencv_features2d \
     -lopencv_flann

HEADERS += \
    video.h \
    v4l2_set.h \
    get_colors.h \
    find_armour.h \
    send_location.h \
    serialport.h
