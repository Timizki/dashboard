#ifndef TEMP_H
#define TEMP_H

#include "qqml.h"
#include <QObject>
#include <QFile>
#include <QSerialPort>

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

    QString getTEMP();
    void setTEMP(QString TEMP);

private:
    QString temp = QString::number(0, 'f', 2) + " °C";
    QFile temperature = QFile("/sys/bus/w1/devices/28-01193a3eb5b5/w1_slave");
};
#endif // TEMP_H
