#include "rpm.h"
#include <QDebug>


RPM::RPM(QObject *parent)
    : QObject{parent}
{
    serialPort.setPortName("/dev/ttyAMA0");
    serialPort.setBaudRate(QSerialPort::Baud9600);
    serialPort.setDataBits(QSerialPort::Data8);
    serialPort.setParity(QSerialPort::NoParity);
    serialPort.setStopBits(QSerialPort::OneStop);
    serialPort.setFlowControl(QSerialPort::NoFlowControl);
    if (!serialPort.open(QIODevice::ReadOnly)) {
        qCritical() << "Failed to open port" << serialPort.portName() << ", error:" << serialPort.errorString();
    }
    QObject::connect(&serialPort, &QSerialPort::readyRead, [this]() {
        QByteArray data = RPM::serialPort.readAll();
        bool ok;
        int value = data.toFloat(&ok);
        if(ok)
            setRPM(value);
        qDebug() << "Received:" << data;
    });
}


RPM::~RPM()
{
    qDebug() << "Cleaning up";

}
float RPM::getRPM()
{
    qDebug() <<" seen " << RPM::rpm;
    return RPM::rpm;
}
void RPM::setRPM(int rpm)
{
    RPM::rpm = rpm;
    qDebug() <<"Setting rpm " << rpm;
    emit RPM::signalRPMUpdate();

}


void RPM::updateRPM()
{

}
