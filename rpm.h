#ifndef RPM_H
#define RPM_H

#include <QObject>
#include <QTimer>
#include <chrono>
#include <gpiod.h>
#include "qqml.h"

class RPM : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float rpm READ rpm WRITE setRpm NOTIFY rpmChanged)
    QML_ELEMENT

public:
    explicit RPM(QObject *parent = nullptr);
    ~RPM() override;

    int rpm() const;
    void setRpm(int value);

signals:
    void rpmChanged();

private slots:
    void updateRpm();

private:
    void initGpio();
    void closeGpio();

    QTimer m_timer;
    int m_rpm = 0;
    int m_pulseCount = 0;
    bool m_lastState = false;
    std::chrono::steady_clock::time_point m_lastUpdate;

    gpiod_chip *m_chip = nullptr;
    gpiod_line *m_line = nullptr;
};

#endif // RPM_H
