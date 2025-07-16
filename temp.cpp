#include "temp.h"
#include <QDebug>
#include "QTimer"

TEMP::TEMP(QObject *parent)
    : QObject{parent}
{
    QObject::connect(&temperature, &QFile::readyRead, [this]() {

        QTextStream in(&temperature);
        QString content = in.readAll();
        temperature.close();
        QString data;
        int index = content.indexOf("t=");
        if (index != -1) {
            QString tempString = content.mid(index + 2);
            double tempC = tempString.toInt() / 1000.0;
            data = QString::number(tempC, 'f', 2) + " °C";
        }
        if(data != nullptr)   {
            setTEMP(data);
            qDebug() << "Received:" << data;
        }
           qDebug() << "NO temp data received";
    });

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this,  &TEMP::updateTEMP);
    timer->start(1000);
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

QString readTEMP() {
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

void TEMP::updateTEMP()
{
    setTEMP(readTEMP());

}
