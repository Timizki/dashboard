#include "lights.h"
#include "QTimer"
#include <QDebug>

Lights::Lights(QObject *parent)
    : QObject{parent}
{

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this,  &Lights::updateStopLight);
    connect(timer, &QTimer::timeout, this,  &Lights::updateOilPressureLight);
    connect(timer, &QTimer::timeout, this,  &Lights::updateTemperatureLight);
    connect(timer, &QTimer::timeout, this,  &Lights::updateBatteryLight);
    timer->start(1000);
}

bool Lights::stop()
{
    return stopState;
}

bool Lights::temperature()
{
    return temperatureState;
}


bool Lights::oilPressure()
{
    return oilPressureState;
}

bool Lights::battery()
{
    return batteryState;
}

void Lights::setStopState(const bool state)
{
    if (state == stopState)
        return;

    stopState = state;
    emit stopStateChanged();
}

void Lights::setTemperatureState(const bool state) {
    if (state == temperatureState)
            return;
    temperatureState = state;
    emit temperatureStateChanged();
}


void Lights::setOilPressureState(const bool state) {
    if (state == oilPressureState)
        return;
    oilPressureState = state;
    emit oilPressureStateChanged();
}


void Lights::setBatteryState(const bool state) {
    if (state == batteryState)
        return;
    batteryState = state;
    emit batteryStateChanged();
}


void Lights::updateStopLight() {
    if(rand()%2 == 1)
        setStopState(false);
    else
        setStopState(true);

}


void Lights::updateOilPressureLight() {
    if(rand()%2 == 1)
        setOilPressureState(false);
    else
        setOilPressureState(true);
}

void Lights::updateTemperatureLight() {
    if(rand()%2 == 1)
        setTemperatureState(false);
    else
        setTemperatureState(true);
}

void Lights:: updateBatteryLight() {
    if(rand()%2 == 1)
        setBatteryState(false);
    else
        setBatteryState(true);
}
