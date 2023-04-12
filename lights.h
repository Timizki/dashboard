#ifndef LIGHTS_H
#define LIGHTS_H

#include "qqml.h"
#include <QObject>
#include <gpiod.h>

class Lights : public QObject
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

public:
    explicit Lights(QObject *parent = nullptr);
    ~Lights();

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


};

#endif // LIGHTS_H
