 #include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QLocale>
#include <QTranslator>
#include "QDebug"
#include <QLoggingCategory>
#include "rpm.h"
#include "temp.h"
#include "lights.h"

int main(int argc, char *argv[])
{
    qmlRegisterType<TEMP>("io.vksn.dashboard", 1, 0, "TEMPERATURE");
    qmlRegisterType<RPM>("io.vksn.dashboard", 1, 0, "RPM");
    qmlRegisterType<Lights>("io.vksn.dashboard", 1, 0, "Lights");
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif    
    qputenv("QT_ASSUME_STDERR_HAS_CONSOLE", "1");

    QGuiApplication app(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "dashboard_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}

