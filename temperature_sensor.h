#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include <QObject>
#include <QTimer>
#include "qqml.h"

class TemperatureSensor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString temperature READ temperature WRITE setTemperature NOTIFY temperatureChanged)
    Q_PROPERTY(QString sensorId READ sensorId WRITE setSensorId NOTIFY sensorIdChanged)
    QML_ELEMENT

public:
    explicit TemperatureSensor(QObject *parent = nullptr);
    ~TemperatureSensor() override = default;

    QString temperature() const;
    void setTemperature(const QString &value);

    QString sensorId() const;
    void setSensorId(const QString &value);

signals:
    void temperatureChanged();
    void sensorIdChanged();

private slots:
    void updateTemperature();

private:
    QString readTemperature() const;

    QString m_temperature;
    QString m_sensorId;
    QTimer m_pollTimer;
};

#endif // TEMPERATURE_SENSOR_H
