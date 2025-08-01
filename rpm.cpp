#include "rpm.h"
#include <QDebug>
#include <QTimer>

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
    qDebug() << "Cleaning up RPM";
    closeGpio();

}

void RPM::initGpio() {
    const char* chipname = "gpiochip0";
    const unsigned int line_num = 14; // GPIO14
qDebug() <<"init rpm " ;
    chip = gpiod_chip_open_by_name(chipname);
    if (!chip) return;

    line = gpiod_chip_get_line(chip, line_num);
    if (!line) return;

    gpiod_line_request_input(line, "rpm");
}

void RPM::closeGpio() {
    if (line) gpiod_line_release(line);
    if (chip) gpiod_chip_close(chip);
}

int RPM::getRPM()
{
    qDebug() <<" seen " << RPM::rpm;
    return RPM::rpm;
}
void RPM::setRPM(int rpm)
{
    RPM::rpm = rpm;
    qDebug() <<"Setting rpm " << rpm;
    emit RPM::RPMUpdated();

}

void RPM::updateRPM() {
    qDebug() <<"reading rpm " ;
    if (!line)
    { 
	    qDebug() << "No RPM line";
	    return;
    }

    qDebug() << "Line found";
    bool currentState = gpiod_line_get_value(line);
    qDebug() << "current state: " << currentState << " LastState: " << lastState;
    if (currentState && !lastState) {
	qDebug() << "RPM state changed";
	RPM::pulseCount++;
    }
    lastState = currentState;

    // Lasketaan RPM 1 sekunnin välein
    auto now = std::chrono::steady_clock::now();
    qDebug() << std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastUpdate).count();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastUpdate).count() >= 1000) {
        int newRpm = RPM::pulseCount * 60;
	qDebug() << "Pulse count = " << RPM::pulseCount;
        if (newRpm != RPM::getRPM()) {
	    RPM::setRPM(newRpm);
        }
        pulseCount = 0;
	m_lastUpdate = now;
    }
}
