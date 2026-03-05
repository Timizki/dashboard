#include "lights.h"
#include <QDebug>
#include <QProcessEnvironment>

namespace {
constexpr int kStopLine = 17;
constexpr int kTemperatureLine = 27;
constexpr int kOilPressureLine = 22;
constexpr int kBatteryLine = 23;
}

Lights::Lights(QObject *parent)
    : QObject{parent},
    chip(nullptr),
    lineStop(nullptr),
    lineTemperature(nullptr),
    lineOilPressure(nullptr),
    lineBattery(nullptr),
    timer(new QTimer(this))
{
    simulationMode = qEnvironmentVariableIntValue("DASHBOARD_SIMULATION") == 1;
    if (!simulationMode) {
        initGpio();
    }

    connect(timer, &QTimer::timeout, this,  &Lights::updateStopLight);
    connect(timer, &QTimer::timeout, this,  &Lights::updateOilPressureLight);
    connect(timer, &QTimer::timeout, this,  &Lights::updateTemperatureLight);
    connect(timer, &QTimer::timeout, this,  &Lights::updateBatteryLight);
    timer->start(1000);
}

Lights::~Lights()
{
    qDebug() << "Release lines";
    closeGpio();
}

void Lights::initGpio()
{
    chip = gpiod_chip_open_by_name(chipname);
    if (!chip) {
        qWarning() << "GPIO chip not available, enabling simulation mode";
        simulationMode = true;
        return;
    }

    lineStop = gpiod_chip_get_line(chip, kStopLine);
    lineTemperature = gpiod_chip_get_line(chip, kTemperatureLine);
    lineOilPressure = gpiod_chip_get_line(chip, kOilPressureLine);
    lineBattery = gpiod_chip_get_line(chip, kBatteryLine);

    if (!lineStop || !lineTemperature || !lineOilPressure || !lineBattery) {
        qWarning() << "Failed to acquire one or more GPIO lines, enabling simulation mode";
        simulationMode = true;
        closeGpio();
        return;
    }

    if (gpiod_line_request_input(lineStop, "dashboard") < 0
        || gpiod_line_request_input(lineTemperature, "dashboard") < 0
        || gpiod_line_request_input(lineOilPressure, "dashboard") < 0
        || gpiod_line_request_input(lineBattery, "dashboard") < 0) {
        qWarning() << "Failed to request GPIO input lines, enabling simulation mode";
        simulationMode = true;
        closeGpio();
        return;
    }

    gpioReady = true;
}

void Lights::closeGpio()
{
    if (lineStop) {
        gpiod_line_release(lineStop);
        lineStop = nullptr;
    }
    if (lineTemperature) {
        gpiod_line_release(lineTemperature);
        lineTemperature = nullptr;
    }
    if (lineOilPressure) {
        gpiod_line_release(lineOilPressure);
        lineOilPressure = nullptr;
    }
    if (lineBattery) {
        gpiod_line_release(lineBattery);
        lineBattery = nullptr;
    }
    if (chip) {
        gpiod_chip_close(chip);
        chip = nullptr;
    }
    gpioReady = false;
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


int Lights::readPin(gpiod_line *line) const
{
    if (simulationMode || !gpioReady || !line) {
        return 0;
    }

    const int val = gpiod_line_get_value(line);
    if (val < 0) {
        qWarning() << "GPIO read failed, returning off state";
        return 0;
    }

    return val;
}

void Lights::updateStopLight()
{
    setStopState(readPin(Lights::lineStop));
}

void Lights::updateOilPressureLight()
{
    setOilPressureState(readPin(Lights::lineOilPressure));
}

void Lights::updateTemperatureLight()
{
    setTemperatureState(readPin(Lights::lineTemperature));
}

void Lights:: updateBatteryLight()
{
    setBatteryState(readPin(Lights::lineBattery));
}
