#ifndef TEMP_H
#define TEMP_H

#include "qqml.h"
#include <QObject>
#include <QFile>
#include <QSerialPort>

class TEMP : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString temp READ readTEMP WRITE setTEMP NOTIFY signalTEMPUpdate)
    Q_PROPERTY(QString sensorId READ getSensorId WRITE setSensorId NOTIFY signalSensorIdChanged)
    QML_ELEMENT

signals:
    void signalTEMPUpdate();
    void signalSensorIdChanged();

public slots:
    void updateTEMP();

public:
    explicit TEMP(QObject *parent = nullptr);
    ~TEMP();

    QString getTEMP();
    void setTEMP(QString TEMP);
    QString getSensorId();
    void setSensorId(QString sensorId);

private:
    QString temp;
    QString sensorId;
    QString  readTEMP();

};
#endif // TEMP_H
