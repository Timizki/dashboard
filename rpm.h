#ifndef RPM_H
#define RPM_H

#include "qqml.h"
#include <QObject>
#include <QTimer>
#include <gpiod.h>

class RPM : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float rpm READ getRPM WRITE setRPM  NOTIFY RPMUpdated)
    QML_ELEMENT

signals:
    void RPMUpdated();

public slots:
    void updateRPM();


public:
    explicit RPM(QObject *parent = nullptr);
    ~RPM();

    int getRPM();
    void setRPM(int rpm);

private:
    QTimer* timer;
    int rpm;
    int pulseCount;
    bool lastState;
    std::chrono::steady_clock::time_point m_lastUpdate;

    gpiod_chip* chip;
    gpiod_line* line;

    int readRPM();
    void initGpio();
    void closeGpio();

};
#endif // RPM_H
