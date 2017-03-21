#-------------------------------------------------
#
# Project created by QtCreator 2017-03-01T15:25:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport

TARGET = McuMonitor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    serialport/bdserialport.cpp \
    comm/bdcomm.cpp \
    comm/commrxthread.cpp \
    comm/commtxthread.cpp \
    comm/commprotocolanalysisthread.cpp \
    iniprofile.cpp \
    commhandlethread.cpp \
    hextobin/hextobindialog.cpp \
    updatefileoperator/updatefileoperator.cpp \
    update/updatedialog.cpp \
    update/updatethread.cpp \
    update/updatedatainfo.cpp \
    aes/advancedencryptionstandard.cpp \
    socket/osal_socket.cpp \
    crc/osal_crc.cpp

HEADERS  += mainwindow.h \
    types.h \
    bd_unit.h \
    bd_function.h \
    bd_optype.h \
    errorcode.h \
    serialport/bdserialport.h \
    comm/bdcomm.h \
    comm/commrxthread.h \
    comm/commtxthread.h \
    comm/commprotocolanalysisthread.h \
    iniprofile.h \
    commhandlethread.h \
    hextobin/hextobindialog.h \
    updatefileoperator/updatefileoperator.h \
    update/updatedialog.h \
    update/updatethread.h \
    update/updatedatainfo.h \
    aes/advancedencryptionstandard.h \
    socket/osal_socket.h \
    crc/osal_crc.h

FORMS    += mainwindow.ui \
    update/updatedialog.ui \
    hextobin/hextobindialog.ui

RESOURCES += \
    mcumonitor.qrc

RC_ICONS = favicon.ico
