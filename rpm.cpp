#include "rpm.h"

#include <QLoggingCategory>

#include <cmath>
#include <ctime>

Q_DECLARE_LOGGING_CATEGORY(gpioLog)

namespace {
constexpr int kRpmLine = 15; // BCM GPIO15, not physical pin 15.
constexpr int kSignalsPerRevolution = 4;
constexpr int kRpmWindowMs = 1000;
}

RPM::RPM(QObject *parent)
    : QObject(parent)
{
    initGpio();
    m_lastUpdate = std::chrono::steady_clock::now();
    connect(&m_timer, &QTimer::timeout, this, &RPM::updateRpm);
    m_timer.start(kRpmWindowMs);
    qCDebug(gpioLog) << "RPM construction passed";
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
    if (!m_line || gpiod_line_request_rising_edge_events(m_line, "rpm") < 0) {
        qCWarning(gpioLog) << "Failed to initialize rpm gpio line for rising edge events";
        closeGpio();
        return;
    }

    const int eventFd = gpiod_line_event_get_fd(m_line);
    if (eventFd < 0) {
        qCWarning(gpioLog) << "Failed to get rpm gpio event fd";
        closeGpio();
        return;
    }

    m_gpioNotifier = new QSocketNotifier(eventFd, QSocketNotifier::Read, this);
    connect(m_gpioNotifier, &QSocketNotifier::activated, this, &RPM::handleGpioEvent);
}

void RPM::closeGpio()
{
    if (m_gpioNotifier) {
        m_gpioNotifier->setEnabled(false);
        delete m_gpioNotifier;
        m_gpioNotifier = nullptr;
    }
    if (m_line) { gpiod_line_release(m_line); m_line = nullptr; }
    if (m_chip) { gpiod_chip_close(m_chip); m_chip = nullptr; }
}

int RPM::rpm() const
{
    return m_rpm;
}

void RPM::setRpm(int value)
{
    if (m_rpm == value) {
        qCDebug(gpioLog) << "RPM value unchanged" << value;
        return;
    }

    qCDebug(gpioLog) << "RPM value changed from" << m_rpm << "to" << value;
    m_rpm = value;
    emit rpmChanged();
}

void RPM::handleGpioEvent()
{
    if (!m_line) return;

    if (m_gpioNotifier) {
        m_gpioNotifier->setEnabled(false);
    }

    while (true) {
        gpiod_line_event event;
        if (gpiod_line_event_read(m_line, &event) < 0) {
            qCWarning(gpioLog) << "Failed to read rpm gpio event";
            break;
        }

        if (event.event_type == GPIOD_LINE_EVENT_RISING_EDGE) {
            ++m_pulseCount;
        }

        const timespec timeout = {0, 0};
        const int pendingEvents = gpiod_line_event_wait(m_line, &timeout);
        if (pendingEvents < 0) {
            qCWarning(gpioLog) << "Failed to wait for pending rpm gpio events";
            break;
        }
        if (pendingEvents == 0) {
            break;
        }
    }

    if (m_gpioNotifier) {
        m_gpioNotifier->setEnabled(true);
    }
}

void RPM::updateRpm()
{
    const auto now = std::chrono::steady_clock::now();
    const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastUpdate).count();
    if (elapsedMs <= 0) return;

    const double elapsedSeconds = static_cast<double>(elapsedMs) / 1000.0;
    const double rpm = (static_cast<double>(m_pulseCount) * 60.0)
        / (static_cast<double>(kSignalsPerRevolution) * elapsedSeconds);
    const int newRpm = static_cast<int>(std::lround(rpm));

    qCDebug(gpioLog) << "RPM calculation pulses" << m_pulseCount
                     << "elapsedMs" << elapsedMs
                     << "rpm" << newRpm;

    setRpm(newRpm);
    m_pulseCount = 0;
    m_lastUpdate = now;
}
