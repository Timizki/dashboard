#ifndef LIGHTS_H
#define LIGHTS_H

#include <QObject>
#include <QTimer>
#include <gpiod.h>
#include "qqml.h"

class Lights : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool stopState READ stop WRITE setStopState NOTIFY stopStateChanged)
    Q_PROPERTY(bool temperatureState READ temperature WRITE setTemperatureState NOTIFY temperatureStateChanged)
    Q_PROPERTY(bool oilPressureState READ oilPressure WRITE setOilPressureState NOTIFY oilPressureStateChanged)
    Q_PROPERTY(bool batteryState READ battery WRITE setBatteryState NOTIFY batteryStateChanged)
    QML_ELEMENT

public:
    explicit Lights(QObject *parent = nullptr);
    ~Lights() override;

    bool stop() const;
    bool temperature() const;
    bool oilPressure() const;
    bool battery() const;

    void setStopState(bool state);
    void setTemperatureState(bool state);
    void setOilPressureState(bool state);
    void setBatteryState(bool state);

signals:
    void stopStateChanged();
    void temperatureStateChanged();
    void oilPressureStateChanged();
    void batteryStateChanged();

private slots:
    void updateStates();

private:
    void initGpio();
    void closeGpio();
    int readPin(gpiod_line *line) const;

    bool m_stopState = false;
    bool m_temperatureState = false;
    bool m_oilPressureState = false;
    bool m_batteryState = false;

    const char *m_chipName = "gpiochip0";
    gpiod_chip *m_chip = nullptr;
    gpiod_line *m_lineStop = nullptr;
    gpiod_line *m_lineTemperature = nullptr;
    gpiod_line *m_lineOilPressure = nullptr;
    gpiod_line *m_lineBattery = nullptr;

    QTimer m_timer;
    bool m_simulationMode = false;
    bool m_gpioReady = false;
};

#endif // LIGHTS_H
