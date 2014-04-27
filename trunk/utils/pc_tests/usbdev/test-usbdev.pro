TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += main.c

build_pass:CONFIG(release, debug|release) {
    DESTDIR = ../build_release
}else{
    DESTDIR = ../build
}

INCLUDEPATH += ../libuuload

win32 {
    INCLUDEPATH += ../../../usbkey/include
    LIBS += -L../../../usbkey/lib/winxp -lusb-1.0
}
unix {
    INCLUDEPATH += /usr/include/libusb-1.0
    LIBS += -L/usr/local/lib -lusb-1.0
}

