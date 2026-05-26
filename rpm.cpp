#include "rpm.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(gpioLog)

namespace {
constexpr int kRpmLine = 14;
constexpr int kSignalsPerRevolution = 4;
constexpr int kPollIntervalMs = 10;
constexpr int kRpmWindowMs = 1000;
}

RPM::RPM(QObject *parent)
    : QObject(parent)
{
    initGpio();
    m_lastUpdate = std::chrono::steady_clock::now();
    connect(&m_timer, &QTimer::timeout, this, &RPM::updateRpm);
    m_timer.start(kPollIntervalMs);
}

RPM::~RPM()
{
    closeGpio();
}

void RPM::initGpio()
{
    m_chip = gpiod_chip_open_by_name("gpiochip0");
    if (!m_chip) {
        qCWarning(gpioLog) << "Failed to open gpio chip";
        return;
    }

    m_line = gpiod_chip_get_line(m_chip, kRpmLine);
    if (!m_line || gpiod_line_request_input(m_line, "rpm") < 0) {
        qCWarning(gpioLog) << "Failed to initialize rpm gpio line";
        closeGpio();
    }
}

void RPM::closeGpio()
{
    if (m_line) { gpiod_line_release(m_line); m_line = nullptr; }
    if (m_chip) { gpiod_chip_close(m_chip); m_chip = nullptr; }
}

int RPM::rpm() const
{
    return m_rpm;
}

void RPM::setRpm(int value)
{
    if (m_rpm == value) return;
    m_rpm = value;
    emit rpmChanged();
}

void RPM::updateRpm()
{
    if (!m_line) return;

    const int lineValue = gpiod_line_get_value(m_line);
    if (lineValue < 0) return;

    const bool currentState = (lineValue != 0);
    if (currentState && !m_lastState) {
        ++m_pulseCount;
    }
    m_lastState = currentState;

    const auto now = std::chrono::steady_clock::now();
    const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastUpdate).count();
    if (elapsedMs < kRpmWindowMs) return;

    const int newRpm = (m_pulseCount * 60) / kSignalsPerRevolution;
    setRpm(newRpm);
    m_pulseCount = 0;
    m_lastUpdate = now;
}
