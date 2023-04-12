#include "lights.h"
#include "QTimer"
#include <QDebug>

Lights::Lights(QObject *parent)
    : QObject{parent}
{

    this->chip = gpiod_chip_open_by_name(chipname);

    this->lineStop = gpiod_chip_get_line(this->chip, 11);
    this->lineTemperature = gpiod_chip_get_line(this->chip, 12);
    this->lineOilPressure = gpiod_chip_get_line(this->chip, 13);
    this->lineBattery = gpiod_chip_get_line(this->chip, 16);


    gpiod_line_request_input(this->lineStop, "dashboard");
    gpiod_line_request_input(this->lineTemperature, "dashboard");
    gpiod_line_request_input(this->lineOilPressure, "dashboard");
    gpiod_line_request_input(this->lineBattery, "dashboard");

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this,  &Lights::updateStopLight);
    connect(timer, &QTimer::timeout, this,  &Lights::updateOilPressureLight);
    connect(timer, &QTimer::timeout, this,  &Lights::updateTemperatureLight);
    connect(timer, &QTimer::timeout, this,  &Lights::updateBatteryLight);
    timer->start(1000);
}

Lights::~Lights()
{
    qDebug() <<"Releas lines" ;
    gpiod_line_release(this->lineStop);
    gpiod_line_release(this->lineTemperature);
    gpiod_line_release(this->lineOilPressure);
    gpiod_line_release(this->lineBattery);
    gpiod_chip_close(this->chip);
}

bool Lights::stop()
{
    return this->stopState;
}

bool Lights::temperature()
{
    return this->temperatureState;
}


bool Lights::oilPressure()
{
    return this->oilPressureState;
}

bool Lights::battery()
{
    return this->batteryState;
}

void Lights::setStopState(const bool state)
{
    if (state == this->stopState)
        return;

    this->stopState = state;
    emit stopStateChanged();
}

void Lights::setTemperatureState(const bool state)
{
    if (state == this->temperatureState)
            return;
   this->temperatureState = state;
   emit temperatureStateChanged();
}


void Lights::setOilPressureState(const bool state)
{
    if (state == this->oilPressureState)
        return;
   this->oilPressureState = state;
   emit oilPressureStateChanged();
}


void Lights::setBatteryState(const bool state)
{
    if (state == batteryState)
        return;
    batteryState = state;
    emit batteryStateChanged();
}


int readPin(gpiod_line* line)
{

    int val = gpiod_line_get_value(line);
    qDebug() << &"did read gpio value " [ val];
     return val;
}

void Lights::updateStopLight()
{
    if(readPin(Lights::lineStop) == 1)
        setStopState(false);
    else
        setStopState(true);
}

void Lights::updateOilPressureLight()
{
    if(readPin(Lights::lineOilPressure) == 1)
        setOilPressureState(false);
    else
        setOilPressureState(true);
}

void Lights::updateTemperatureLight()
{
    if(readPin(Lights::lineTemperature) == 1)
        setTemperatureState(false);
    else
        setTemperatureState(true);
}

void Lights:: updateBatteryLight()
{
    if(readPin(Lights::lineBattery) == 1)
        setBatteryState(false);
    else
        setBatteryState(true);
}
