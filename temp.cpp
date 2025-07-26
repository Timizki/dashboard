#include "temp.h"
#include <QDebug>
#include "QTimer"

TEMP::TEMP(QObject *parent)
    : QObject{parent}
{
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this,  &TEMP::readTEMP);
    timer->start(1000);
}

TEMP::~TEMP()
{
    qDebug() << "Cleaning up";
}

QString TEMP::getTEMP()
{
    return TEMP::temp;
}

void TEMP::setTEMP(QString temperature)
{
    TEMP::temp = temperature;
    qDebug() <<"Setting temp " << temperature;
    emit TEMP::signalTEMPUpdate();

}

void TEMP::setSensorId(QString sensorId) {
    TEMP::sensorId = sensorId;
}
QString TEMP::getSensorId() {
    return TEMP::sensorId;
}

QString TEMP::readTEMP() {

    QFile file("/sys/bus/w1/devices/"+ TEMP::getSensorId() +"/w1_slave");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return QString("Error");

    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    int index = content.indexOf("t=");

    if (index != -1) {
        QString tempString = content.mid(index + 2);
        double tempC = tempString.toInt() / 1000.0;
        return QString::number(tempC, 'f', 2) + " °C";
    }
    return QString("Invalid value");
}

void TEMP::updateTEMP()
{
    setTEMP(readTEMP());
}
