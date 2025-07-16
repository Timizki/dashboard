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
    qDebug() <<" seen " << TEMP::temp;
    return  TEMP::temp;
}
void TEMP::setTEMP(QString temp)
{
    TEMP::temp= temp;
    qDebug() <<"Setting temp " << temp;
    emit TEMP::signalTEMPUpdate();

}

void TEMP::updateTEMP()
{

}
