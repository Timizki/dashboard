import QtQuick 2.15
import QtQuick.Window 2.15
import io.vksn.dashboard 1.0
import QtQuick.Extras 1.4
import QtQuick.Controls.Styles 1.4
Window {
    Lights {
           id: lights
       }
    RPM {
        id: rpmConn;
    }
    function onSignalRPMUpdate() {
                console.log("Received data:",  rpmConn.rpm);
            rpm.text = rpmConn.rpm;
    }


    id: dash
    width: 400 
    height: 1280
    
    visible: true
    title: qsTr("Dashboard")
    Rectangle {
        id: container
      	
	anchors.centerIn: parent
        rotation: 90
        transformOrigin: Item.Center
        width: parent.height
	height: parent.width  
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

        }

        AnimatedImage {
            id: outboard
            source: "images/outboard.png"
            height: 300
            width: 250
            anchors.left: container.left
            anchors.leftMargin: 55
            y: 50
            fillMode: Image.PreserveAspectFit
            transformOrigin: Item.TopRight
        }
        Canvas {
            width: 450
            height: 300
            contextType: "2d"

            Path {
                id: waterline
                startX: 100
                startY: 175

                PathCurve { x: 150; y: 150 }
                PathCurve { x: 225; y: 175 }
                PathCurve { x: 300; y: 150 }
                PathCurve { x: 350; y: 175 }

            }

            onPaint: {
                context.strokeStyle = Qt.rgba(.4,.6,.8);
                context.lineWidth = 3
                context.path = waterline;

                context.stroke();
            }
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


        Rectangle {
            id: lightsContainer
            anchors.left: container.horizontalCenter
            Text {
                id: rpm
                text: rpmConn.rpm
                color: "white"
                font.family: "Helvetica"
                font.pixelSize: 50
                font.bold: Font.ExtraBold
                font.capitalization: Font.AllUppercase
                anchors.horizontalCenter:  lightsContainer.horizontalCenter
                y: 25


            }

            Image {
                id: stop
                source: lights.stopState ? "images/stop_on.png" : "images/stop_off.png"                
                y: 100
                height: 50
                width: 100                
                fillMode: Image.PreserveAspectFit
                anchors.horizontalCenter:  lightsContainer.horizontalCenter

            }

            Image {
                id: temperature
                source: lights.temperatureState ? "images/temperature_on.png" : "images/temperature_off.png"
                y: 175
                height: 50
                width: 100
                fillMode: Image.PreserveAspectFit
                anchors.horizontalCenter:  lightsContainer.horizontalCenter
            }

            Image {
                id: oil_pressure
                source: lights.oilPressureState ? "images/oil_pressure_on.png" : "images/oil_pressure_off.png"
                y: 250
                height: 50
                width: 100
                fillMode: Image.PreserveAspectFit
                anchors.horizontalCenter:  lightsContainer.horizontalCenter

            }

            Image {
                id: battery
                source: lights.batteryState ? "images/battery_on.png" : "images/battery_off.png"
                y: 325
                height: 50
                width: 100
                fillMode: Image.PreserveAspectFit
                anchors.horizontalCenter:  lightsContainer.horizontalCenter
            }
        }

        Rectangle {
            id: compass_background
            anchors.right: container.right;
            anchors.rightMargin: 55
            y: 5
            width: 390
            height: 390
            radius: width*0.5
            color: "steelblue"
            SequentialAnimation on rotation {
                running: true
                loops: Animation.Infinite

                RotationAnimation {
                       from: -85
                       to: 85
                       duration: 10000
                }
                RotationAnimation {
                       from: 85
                       to: -85
                       duration: 10000
                }
            }


            Text {
                anchors.horizontalCenter: compass_background.horizontalCenter
                anchors.top: compass_background.top
                anchors.topMargin: 10
                font.family: "Helvetica"
                font.pixelSize: 50
                font.bold: Font.ExtraBold
                font.capitalization: Font.AllUppercase
                color: "white"
                text: "N"

            }
            Text {
                anchors.right: compass_background.right
                anchors.verticalCenter: compass_background.verticalCenter
                anchors.rightMargin: 10
                font.family: "Helvetica"
                font.pixelSize: 50
                font.bold: Font.ExtraBold
                font.capitalization: Font.AllUppercase
                color: "white"
                text: "E"
            }
            Text {
                anchors.bottom: compass_background.bottom
                anchors.horizontalCenter: compass_background.horizontalCenter
                font.family: "Helvetica"
                font.pixelSize: 50
                font.bold: Font.ExtraBold
                font.capitalization: Font.AllUppercase
                color: "white"
                text: "S"
            }

            Text {
                id: compass_west
                anchors.left: compass_background.left
                anchors.verticalCenter: compass_background.verticalCenter
                anchors.leftMargin: 10
                font.family: "Helvetica"
                font.pixelSize: 50
                font.bold: Font.ExtraBold
                font.capitalization: Font.AllUppercase
                color: "white"
                text: "W"
            }


         }

        Canvas {
            id: arrow
            anchors.horizontalCenter:   compass_background.horizontalCenter
            y: 75

            height: 20
            width: 50

            antialiasing: true

            onPaint: {
                var ctx = arrow.getContext('2d')

                ctx.strokeStyle = "#000000"
                ctx.lineWidth = 6
                ctx.beginPath()
                ctx.moveTo(arrow.width * 0.05, arrow.height)
                ctx.lineTo(arrow.width / 2, arrow.height * 0.1)
                ctx.lineTo(arrow.width * 0.95, arrow.height)
                ctx.stroke()
            }
        }

    }
}


