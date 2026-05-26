#ifndef WARNING_SIGNALS_H
#define WARNING_SIGNALS_H

#include "qqml.h"
#include <QObject>
#include <gpiod.h>
#include <QTimer>

class WarningSignals : public QObject
{

    Q_OBJECT
    Q_PROPERTY(bool stopState READ stop WRITE setStopState NOTIFY stopStateChanged)
    Q_PROPERTY(bool temperatureState READ temperature WRITE setTemperatureState NOTIFY temperatureStateChanged)
    Q_PROPERTY(bool oilPressureState READ oilPressure WRITE setOilPressureState NOTIFY oilPressureStateChanged)
    Q_PROPERTY(bool batteryState READ battery WRITE setBatteryState NOTIFY batteryStateChanged)
    QML_ELEMENT



signals:
    void stopStateChanged() ;
    void temperatureStateChanged();
    void oilPressureStateChanged();
    void batteryStateChanged();

 public slots:
    void updateStopLight();
    void updateOilPressureLight();
    void updateTemperatureLight();
    void updateBatteryLight();
    void updateShutdownSignal();

public:
    explicit WarningSignals(QObject *parent = nullptr);
    ~WarningSignals();

    bool stop();
    bool temperature();
    bool oilPressure();
    bool battery();

    void setStopState(const bool state);
    void setTemperatureState(const bool state);
    void setOilPressureState(const bool state);
    void setBatteryState(const bool state);

private:
    bool stopState = false;
    bool temperatureState = false;
    bool oilPressureState = false;
    bool batteryState = false;

    const char *chipname = "gpiochip0";
    struct gpiod_chip *chip;
    struct gpiod_line *lineStop;
    struct gpiod_line *lineTemperature;
    struct gpiod_line *lineOilPressure;
    struct gpiod_line *lineBattery;
    struct gpiod_line *lineShutdown;

    QTimer *timer;
    bool simulationMode = false;
    bool gpioReady = false;
    int shutdownLowSeconds = 0;
    bool shutdownRequested = false;

    void initGpio();
    void closeGpio();
    int readPin(gpiod_line *line) const;


};

#endif // WARNING_SIGNALS_H
