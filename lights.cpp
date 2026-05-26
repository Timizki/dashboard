#include "lights.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(gpioLog)

namespace {
constexpr int kStopLine = 17;
constexpr int kTemperatureLine = 27;
constexpr int kOilPressureLine = 22;
constexpr int kBatteryLine = 23;
constexpr int kPollIntervalMs = 1000;
}

Lights::Lights(QObject *parent)
    : QObject(parent)
{
    if (qEnvironmentVariableIntValue("DASHBOARD_SIMULATION") == 1) {
        m_simulationMode = true;
    } else {
        initGpio();
    }

    connect(&m_timer, &QTimer::timeout, this, &Lights::updateStates);
    m_timer.start(kPollIntervalMs);
}

Lights::~Lights()
{
    closeGpio();
}

bool Lights::stop() const { return m_stopState; }
bool Lights::temperature() const { return m_temperatureState; }
bool Lights::oilPressure() const { return m_oilPressureState; }
bool Lights::battery() const { return m_batteryState; }

void Lights::setStopState(bool state)
{
    if (state == m_stopState) return;
    m_stopState = state;
    emit stopStateChanged();
}

void Lights::setTemperatureState(bool state)
{
    if (state == m_temperatureState) return;
    m_temperatureState = state;
    emit temperatureStateChanged();
}

void Lights::setOilPressureState(bool state)
{
    if (state == m_oilPressureState) return;
    m_oilPressureState = state;
    emit oilPressureStateChanged();
}

void Lights::setBatteryState(bool state)
{
    if (state == m_batteryState) return;
    m_batteryState = state;
    emit batteryStateChanged();
}

void Lights::updateStates()
{
    setStopState(readPin(m_lineStop));
    setTemperatureState(readPin(m_lineTemperature));
    setOilPressureState(!readPin(m_lineOilPressure)); // active-low
    setBatteryState(readPin(m_lineBattery));
}

void Lights::initGpio()
{
    m_chip = gpiod_chip_open_by_name(m_chipName);
    if (!m_chip) {
        qCWarning(gpioLog) << "GPIO chip not available, enabling simulation mode";
        m_simulationMode = true;
        return;
    }

    m_lineStop = gpiod_chip_get_line(m_chip, kStopLine);
    m_lineTemperature = gpiod_chip_get_line(m_chip, kTemperatureLine);
    m_lineOilPressure = gpiod_chip_get_line(m_chip, kOilPressureLine);
    m_lineBattery = gpiod_chip_get_line(m_chip, kBatteryLine);

    if (!m_lineStop || !m_lineTemperature || !m_lineOilPressure || !m_lineBattery
        || gpiod_line_request_input(m_lineStop, "dashboard") < 0
        || gpiod_line_request_input(m_lineTemperature, "dashboard") < 0
        || gpiod_line_request_input(m_lineOilPressure, "dashboard") < 0
        || gpiod_line_request_input(m_lineBattery, "dashboard") < 0) {
        qCWarning(gpioLog) << "Failed to initialize GPIO lines, enabling simulation mode";
        m_simulationMode = true;
        closeGpio();
        return;
    }

    m_gpioReady = true;
}

void Lights::closeGpio()
{
    if (m_lineStop) { gpiod_line_release(m_lineStop); m_lineStop = nullptr; }
    if (m_lineTemperature) { gpiod_line_release(m_lineTemperature); m_lineTemperature = nullptr; }
    if (m_lineOilPressure) { gpiod_line_release(m_lineOilPressure); m_lineOilPressure = nullptr; }
    if (m_lineBattery) { gpiod_line_release(m_lineBattery); m_lineBattery = nullptr; }
    if (m_chip) { gpiod_chip_close(m_chip); m_chip = nullptr; }
    m_gpioReady = false;
}

int Lights::readPin(gpiod_line *line) const
{
    if (m_simulationMode || !m_gpioReady || !line) return 0;

    const int val = gpiod_line_get_value(line);
    if (val < 0) {
        qCWarning(gpioLog) << "GPIO read failed, returning off state";
        return 0;
    }
    return val;
}
