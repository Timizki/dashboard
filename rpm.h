#ifndef RPM_H
#define RPM_H

#include "qqml.h"
#include <QObject>
#include <QSerialPort>

class RPM : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float rpm READ getRPM WRITE setRPM  NOTIFY signalRPMUpdate)
    QML_ELEMENT

signals:
    void signalRPMUpdate();

public slots:
    void updateRPM();


public:
    explicit RPM(QObject *parent = nullptr);
    ~RPM();

    float getRPM();
    void setRPM(int rpm);

private:
    int rpm = 0;
    QSerialPort serialPort;

};
#endif // RPM_H
