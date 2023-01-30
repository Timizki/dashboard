import QtQuick 2.15
import QtQuick.Window 2.15


Window {
    width: 1280
    height: 400
    visible: true
    title: qsTr("Dashboard")    

    Rectangle {
        anchors.fill: parent
               color: "black"
        focus: true
        Keys.onLeftPressed: {
            down_trim.stop()
            up_trim.start()
        }
        Keys.onRightPressed: {
            up_trim.stop()
            down_trim.start()
        }

        Keys.onDigit1Pressed: {
            temperature.source="images/temperature_on.png"
        }
        Keys.onDigit2Pressed: {
            oil_pressure.source="images/oil_pressure_on.png"
        }
        Keys.onDigit3Pressed: {
            battery.source="images/battery_on.png"
        }

        AnimatedImage {
            id: outboard
            source: "images/outboard.png"
            height: 300
            width: 250
            x: 100
            y: 50
            fillMode: Image.PreserveAspectFit
            transformOrigin: Item.TopRight
        }

        RotationAnimator {
           id: down_trim
           target: outboard
           from:  outboard.rotation
           to: 0
           duration: 1000
        }

        RotationAnimator {
           id: up_trim
           target: outboard
           from: outboard.rotation
           to: 33
           duration: 1000
       }

        Image {
            id: stop
            source: "images/temperature_on.png"
            x: 500
            y: 25
            height: 50
            width: 100
            fillMode: Image.PreserveAspectFit
        }

        Image {
            id: temperature
            source: "images/temperature_off.png"
            x: 500
            y: 125
            height: 50
            width: 100
            fillMode: Image.PreserveAspectFit
        }

        Image {
            id: oil_pressure
            source: "images/oil_pressure_off.png"
            x: 500
            y: 225
            height: 50
            width: 100
            fillMode: Image.PreserveAspectFit

        }

        Image {
            id: battery
            source: "images/battery_off.png"
            x: 500
            y: 325
            height: 50
            width: 100
            fillMode: Image.PreserveAspectFit

        }


    }
}
