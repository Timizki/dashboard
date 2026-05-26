#include "warning_signals.h"
#include <QProcessEnvironment>
#include <QLoggingCategory>
#include <QProcess>

Q_DECLARE_LOGGING_CATEGORY(gpioLog)

namespace {
constexpr int kStopLine = 17;
constexpr int kTemperatureLine = 27;
constexpr int kOilPressureLine = 22;
constexpr int kBatteryLine = 23;
constexpr int kShutdownLine = 3;
constexpr int kShutdownHoldSeconds = 5;
}

WarningSignals::WarningSignals(QObject *parent)
    : QObject{parent},
    chip(nullptr),
    lineStop(nullptr),
    lineTemperature(nullptr),
    lineOilPressure(nullptr),
    lineBattery(nullptr),
    lineShutdown(nullptr),
    timer(new QTimer(this))
{
    simulationMode = qEnvironmentVariableIntValue("DASHBOARD_SIMULATION") == 1;
    if (!simulationMode) {
        initGpio();
    }

    connect(timer, &QTimer::timeout, this,  &WarningSignals::updateStopLight);
    connect(timer, &QTimer::timeout, this,  &WarningSignals::updateOilPressureLight);
    connect(timer, &QTimer::timeout, this,  &WarningSignals::updateTemperatureLight);
    connect(timer, &QTimer::timeout, this,  &WarningSignals::updateBatteryLight);
    connect(timer, &QTimer::timeout, this,  &WarningSignals::updateShutdownSignal);
    timer->start(1000);
}

WarningSignals::~WarningSignals()
{
    qCDebug(gpioLog) << "Release lines";
    closeGpio();
}

void WarningSignals::initGpio()
{
    qCDebug(gpioLog) << "Initializing lights GPIO on chip" << chipname;
    chip = gpiod_chip_open_by_name(chipname);
    if (!chip) {
        qCWarning(gpioLog) << "GPIO chip not available, enabling simulation mode";
        simulationMode = true;
        return;
    }

    lineStop = gpiod_chip_get_line(chip, kStopLine);
    lineTemperature = gpiod_chip_get_line(chip, kTemperatureLine);
    lineOilPressure = gpiod_chip_get_line(chip, kOilPressureLine);
    lineBattery = gpiod_chip_get_line(chip, kBatteryLine);
    lineShutdown = gpiod_chip_get_line(chip, kShutdownLine);

    if (!lineStop || !lineTemperature || !lineOilPressure || !lineBattery || !lineShutdown) {
        qCWarning(gpioLog) << "Failed to acquire one or more GPIO lines, enabling simulation mode";
        simulationMode = true;
        closeGpio();
        return;
    }

    if (gpiod_line_request_input(lineStop, "dashboard") < 0
        || gpiod_line_request_input(lineTemperature, "dashboard") < 0
        || gpiod_line_request_input(lineOilPressure, "dashboard") < 0
        || gpiod_line_request_input(lineBattery, "dashboard") < 0
        || gpiod_line_request_input(lineShutdown, "dashboard") < 0) {
        qCWarning(gpioLog) << "Failed to request GPIO input lines, enabling simulation mode";
        simulationMode = true;
        closeGpio();
        return;
    }

    gpioReady = true;
}

void WarningSignals::closeGpio()
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
    if (lineShutdown) {
        gpiod_line_release(lineShutdown);
        lineShutdown = nullptr;
    }
    if (chip) {
        gpiod_chip_close(chip);
        chip = nullptr;
    }
    gpioReady = false;
}

bool WarningSignals::stop()
{
    return this->stopState;
}

bool WarningSignals::temperature()
{
    return this->temperatureState;
}


bool WarningSignals::oilPressure()
{
    return this->oilPressureState;
}

bool WarningSignals::battery()
{
    return this->batteryState;
}

void WarningSignals::setStopState(const bool state)
{
    if (state == this->stopState)
        return;

    this->stopState = state;
    emit stopStateChanged();
}

void WarningSignals::setTemperatureState(const bool state)
{
    if (state == this->temperatureState)
            return;
   this->temperatureState = state;
   emit temperatureStateChanged();
}


void WarningSignals::setOilPressureState(const bool state)
{
    if (state == this->oilPressureState)
        return;
   this->oilPressureState = state;
   emit oilPressureStateChanged();
}


void WarningSignals::setBatteryState(const bool state)
{
    if (state == batteryState)
        return;
    batteryState = state;
    emit batteryStateChanged();
}


int WarningSignals::readPin(gpiod_line *line) const
{
    if (simulationMode || !gpioReady || !line) {
        return 0;
    }

    const int val = gpiod_line_get_value(line);
    qCDebug(gpioLog) << "Read gpio line" << gpiod_line_offset(line) << "value" << val;
    if (val < 0) {
        qCWarning(gpioLog) << "GPIO read failed, returning off state";
        return 0;
    }

    return val;
}

void WarningSignals::updateStopLight()
{
    setStopState(readPin(WarningSignals::lineStop));
}

void WarningSignals::updateOilPressureLight()
{
    // Oil pressure input is active-low on the harness: low means warning ON.
    setOilPressureState(!readPin(WarningSignals::lineOilPressure));
}

void WarningSignals::updateTemperatureLight()
{
    setTemperatureState(readPin(WarningSignals::lineTemperature));
}


void WarningSignals::updateShutdownSignal()
{
    if (simulationMode || !gpioReady || shutdownRequested) {
        return;
    }

    const int pinValue = readPin(lineShutdown);
    if (pinValue == 0) {
        ++shutdownLowSeconds;
        qCDebug(gpioLog) << "GPIO3 low for" << shutdownLowSeconds << "second(s)";
        if (shutdownLowSeconds >= kShutdownHoldSeconds) {
            shutdownRequested = true;
            qCWarning(gpioLog) << "GPIO3 held low for" << kShutdownHoldSeconds << "seconds; requesting shutdown";
            QProcess::startDetached(QStringLiteral("/usr/bin/sudo"),
                                    QStringList{QStringLiteral("/sbin/shutdown"),
                                                QStringLiteral("-h"),
                                                QStringLiteral("now")});
        }
        return;
    }

    if (shutdownLowSeconds != 0) {
        qCDebug(gpioLog) << "GPIO3 returned high; resetting shutdown timer";
    }
    shutdownLowSeconds = 0;
}
void WarningSignals:: updateBatteryLight()
{
    setBatteryState(readPin(WarningSignals::lineBattery));
}
