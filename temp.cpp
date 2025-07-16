#include "temp.h"
#include <QDebug>



TEMP::TEMP(QObject *parent)
    : QObject{parent}
{
    QObject::connect(tempSensor, &DS18B20::getTemp, [this]() {
        float data = tempSensor->getUnits();
        setTEMP(data);
        qDebug() << "Received:" << data;
    });
}


TEMP::~TEMP()
{
    qDebug() << "Cleaning up";

}
float TEMP::getTEMP()
{
    qDebug() <<" seen " << TEMP::temp;
    return TEMP::temp;
}
void TEMP::setTEMP(int temp)
{
    TEMP::temp= temp;
    qDebug() <<"Setting temp " << temp;
    emit TEMP::signalTEMPUpdate();

}


void TEMP::updateTEMP()
{

}
