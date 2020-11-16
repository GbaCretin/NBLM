QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += \
    c++1z

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    OPNBInterface.cpp \
    chip/mame/2610intf.c \
    chip/mame/ay8910.c \
    chip/mame/emu2149.c \
    chip/mame/fm.c \
    chip/mame/ymdeltat.c \
    chip/opnb.cpp \
    chip/resampler.cpp \
    log.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    OPNBInterface.hpp \
    audioDef.hpp \
    chip/audio_def.hpp \
    chip/chip_misc.hpp \
    chip/mame/2610intf.h \
    chip/mame/ay8910.h \
    chip/mame/emu2149.h \
    chip/mame/emutypes.h \
    chip/mame/fm.h \
    chip/mame/mamedef.h \
    chip/mame/ymdeltat.h \
    chip/opnb.hpp \
    chip/resampler.hpp \
    log.hpp \
    mainwindow.hpp \
    math.hpp

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
