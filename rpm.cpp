#include "rpm.h"
#include <QTimer>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(gpioLog)

RPM::RPM(QObject *parent)
    : QObject(parent),
    timer(new QTimer(this)),
    rpm(0),
    pulseCount(0),
    lastState(false),
    chip(nullptr),
    line(nullptr)
{
    initGpio();
    m_lastUpdate = std::chrono::steady_clock::now();
    connect(timer, &QTimer::timeout, this, &RPM::updateRPM);
    timer->start(10); // Pollausnopeus: 10 ms

}


RPM::~RPM()
{
    qCDebug(gpioLog) << "Cleaning up RPM";
    closeGpio();

}

void RPM::initGpio() {
    const char* chipname = "gpiochip0";
    const unsigned int line_num = 14; // GPIO14
    qCDebug(gpioLog) << "init rpm, chip" << chipname << "line" << line_num;
    chip = gpiod_chip_open_by_name(chipname);
    if (!chip) {
        qCWarning(gpioLog) << "Failed to open gpio chip" << chipname;
        return;
    }

    line = gpiod_chip_get_line(chip, line_num);
    if (!line) {
        qCWarning(gpioLog) << "Failed to get rpm gpio line" << line_num;
        return;
    }

    if (gpiod_line_request_input(line, "rpm") < 0) {
        qCWarning(gpioLog) << "Failed to request rpm gpio line as input" << line_num;
    }
}

void RPM::closeGpio() {
    if (line) gpiod_line_release(line);
    if (chip) gpiod_chip_close(chip);
}

int RPM::getRPM()
{
    return RPM::rpm;
}
void RPM::setRPM(int rpm)
{
    RPM::rpm = rpm;
    qCDebug(gpioLog) << "Setting rpm" << rpm;
    emit RPM::RPMUpdated();

}

void RPM::updateRPM() {
    qCDebug(gpioLog) << "reading rpm";
    if (!line)
    { 
	    qCWarning(gpioLog) << "No RPM line";
	    return;
    }

    qCDebug(gpioLog) << "Line found";
    const int lineValue = gpiod_line_get_value(line);
    if (lineValue < 0) {
        qCWarning(gpioLog) << "Failed to read rpm gpio line";
        return;
    }

    const bool currentState = (lineValue != 0);
    qCDebug(gpioLog) << "current state:" << currentState << "LastState:" << lastState;
    if (currentState && !lastState) {
	qCDebug(gpioLog) << "RPM state changed";
	RPM::pulseCount++;
    }
    lastState = currentState;

    // Lasketaan RPM 1 sekunnin välein
    auto now = std::chrono::steady_clock::now();
    qCDebug(gpioLog) << "elapsed ms" << std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastUpdate).count();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastUpdate).count() >= 1000) {
        int newRpm = RPM::pulseCount * 60;
	qCDebug(gpioLog) << "Pulse count =" << RPM::pulseCount;
        if (newRpm != RPM::rpm) {
	    RPM::setRPM(newRpm);
        }
        pulseCount = 0;
	m_lastUpdate = now;
    }
}
