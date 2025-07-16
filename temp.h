#ifndef TEMP_H
#define TEMP_H

#include "qqml.h"
#include <QObject>
#include <QSerialPort>
#include "DS18B20.h"

class TEMP : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float temp READ getTEMP WRITE setTEMP NOTIFY signalTEMPUpdate)
    QML_ELEMENT

signals:
    void signalTEMPUpdate();

public slots:
    void updateTEMP();


public:
    explicit TEMP(QObject *parent = nullptr);
    ~TEMP();

    float getTEMP();
    void setTEMP(int TEMP);

private:
    int temp = 0;
    DS18B20 *tempSensor;
};
#endif // TEMP_H
