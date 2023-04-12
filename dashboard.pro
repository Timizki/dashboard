QT += quick

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        lights.cpp \
        main.cpp

RESOURCES += qml.qrc

TRANSLATIONS += \
    dashboard_fi_FI.ts
CONFIG += lrelease
CONFIG += embed_translations
CONFIG += console
CONFIG += qmltypes

QML_IMPORT_NAME = io.vksn.dashboard
QML_IMPORT_MAJOR_VERSION = 1.0

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    images/outboard.png

HEADERS += \
    lights.h

unix:!macx: LIBS += -lgpiod
