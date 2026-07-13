#include "gpio_logger.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLoggingCategory>
#include <QMutex>
#include <QMutexLocker>
#include <QStandardPaths>
#include <QtGlobal>

#include <cstdlib>

Q_LOGGING_CATEGORY(gpioLog, "gpio")

namespace {
QFile *g_logFile = nullptr;
QMutex g_logMutex;
QtMessageHandler g_prevHandler = nullptr;
bool g_handlerInstalled = false;

QString messageTypeToString(QtMsgType type)
{
    switch (type) {
    case QtDebugMsg: return "DEBUG";
    case QtInfoMsg: return "INFO";
    case QtWarningMsg: return "WARN";
    case QtCriticalMsg: return "ERROR";
    case QtFatalMsg: return "FATAL";
    }
    return "UNKNOWN";
}

void gpioMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    const QString category = QString::fromUtf8(context.category);
    if (!category.startsWith(QStringLiteral("gpio"))) {
        if (g_prevHandler) {
            g_prevHandler(type, context, msg);
        }
        return;
    }

    {
        QMutexLocker locker(&g_logMutex);
        if (g_logFile && g_logFile->isOpen()) {
            const QString line = QStringLiteral("%1 [%2] [%3] %4\n")
                                     .arg(QDateTime::currentDateTime().toString(Qt::ISODateWithMs))
                                     .arg(messageTypeToString(type), category, msg);
            g_logFile->write(line.toUtf8());
        }
    }

    if (g_prevHandler) {
        g_prevHandler(type, context, msg);
    }
}


void gpioShutdownAtExit()
{
    GpioLogger::shutdown();
}
}

namespace GpioLogger {

QString logFilePath()
{
    const QString baseDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (!baseDir.isEmpty()) {
        return baseDir + QDir::separator() + QStringLiteral("gpio.log");
    }
    return QStringLiteral("./gpio.log");
}

void shutdown()
{
    QMutexLocker locker(&g_logMutex);
    if (g_handlerInstalled) {
        qInstallMessageHandler(g_prevHandler);
        g_handlerInstalled = false;
    }
    if (g_logFile) {
        if (g_logFile->isOpen()) {
            g_logFile->close();
        }
        delete g_logFile;
        g_logFile = nullptr;
    }
}

void init()
{
    static bool initialized = false;
    if (initialized) {
        return;
    }

    const QString path = logFilePath();
    const QFileInfo fi(path);
    QDir().mkpath(fi.absolutePath());

    g_logFile = new QFile(path);
    if (!g_logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        delete g_logFile;
        g_logFile = nullptr;
        return;
    }

    QLoggingCategory::setFilterRules(QStringLiteral("gpio*.debug=true\ngpio*.info=true\ngpio*.warning=true\ngpio*.critical=true"));
    g_prevHandler = qInstallMessageHandler(gpioMessageHandler);
    g_handlerInstalled = true;
    std::atexit(gpioShutdownAtExit);
    initialized = true;
}

}
