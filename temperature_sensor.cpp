#include "temperature_sensor.h"

#include <QFile>
#include <QTextStream>

namespace {
constexpr int kPollIntervalMs = 1000;
}

TemperatureSensor::TemperatureSensor(QObject *parent)
    : QObject(parent)
{
    connect(&m_pollTimer, &QTimer::timeout, this, &TemperatureSensor::updateTemperature);
    m_pollTimer.start(kPollIntervalMs);
}

QString TemperatureSensor::temperature() const
{
    return m_temperature;
}

void TemperatureSensor::setTemperature(const QString &value)
{
    if (m_temperature == value) return;
    m_temperature = value;
    emit temperatureChanged();
}

QString TemperatureSensor::sensorId() const
{
    return m_sensorId;
}

void TemperatureSensor::setSensorId(const QString &value)
{
    if (m_sensorId == value) return;
    m_sensorId = value;
    emit sensorIdChanged();
}

QString TemperatureSensor::readTemperature() const
{
    QFile sensorFile("/sys/bus/w1/devices/" + m_sensorId + "/w1_slave");
    if (!sensorFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QStringLiteral("Error");
    }

    QTextStream in(&sensorFile);
    const QString rawSensorContent = in.readAll();

    const int temperatureMarkerIndex = rawSensorContent.indexOf("t=");
    if (temperatureMarkerIndex == -1) {
        return QStringLiteral("Invalid value");
    }

    const QString milliCelsiusText = rawSensorContent.mid(temperatureMarkerIndex + 2);
    const double celsius = milliCelsiusText.toInt() / 1000.0;
    return QString::number(celsius, 'f', 2) + QStringLiteral(" °C");
}

void TemperatureSensor::updateTemperature()
{
    setTemperature(readTemperature());
}
