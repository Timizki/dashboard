#include "temp.h"
#include <QDebug>
#include "QTimer"

TEMP::TEMP(QObject *parent)
    : QObject{parent}
{

}

TEMP::~TEMP()
{
    qDebug() << "Cleaning up";
}

QString TEMP::getTEMP()
{
    QFile file("/sys/bus/w1/devices/28-01193a3eb5b5/w1_slave"); // Vaihda oikeaan ID:hen
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return "Error";

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    int index = content.indexOf("t=");
    if (index != -1) {
        QString tempString = content.mid(index + 2);
        double tempC = tempString.toInt() / 1000.0;
        return QString::number(tempC, 'f', 2) + " °C";
    }
    return "Invalid data";
}

void TEMP::setTEMP(QString temp)
{
    TEMP::temp= temp;
    qDebug() <<"Setting temp " << temp;
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
        return "Error";

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    int index = content.indexOf("t=");
    if (index != -1) {
        QString tempString = content.mid(index + 2);
        double tempC = tempString.toInt() / 1000.0;
        return QString::number(tempC, 'f', 2) + " °C";
    }
    return "Invalid data";
}

void TEMP::updateTEMP()
{
    setTEMP(readTEMP());
}
