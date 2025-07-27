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
    const unsigned int line_num = 17; // GPIO17
qDebug() <<"init rpm " ;
    chip = gpiod_chip_open_by_name(chipname);
    if (!chip) return;

    line = gpiod_chip_get_line(chip, line_num);
    if (!line) return;

    gpiod_line_request_input(line, "rpmsensor");
}

void RPM::closeGpio() {
    if (line) gpiod_line_release(line);
    if (chip) gpiod_chip_close(chip);
}

float RPM::getRPM()
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

    return;

    bool currentState = gpiod_line_get_value(line);
    if (currentState && !lastState) {
        pulseCount++;
    }
    lastState = currentState;

    // Lasketaan RPM 1 sekunnin välein
    static int elapsed = 0;
    elapsed += 10;
    if (elapsed >= 1000) {
        int newRpm = pulseCount * 60;
        if (newRpm != rpm) {
            rpm = newRpm;
            emit RPMUpdated();
        }
        pulseCount = 0;
        elapsed = 0;
    }
}
