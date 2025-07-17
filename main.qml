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

    TEMP_INTAKE {
        id: intakeTemp;
        sensorId: "28-01193a3eb5b5";
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
            id: rpmGauge
            anchors.right: container.right;
            anchors.rightMargin: 55
            y: 5
            width: 390
            height: 390
            radius: width*0.5
            color: "steelblue"            

            Text {
                id: label_outlet
                anchors.right: rpmGauge.right
                anchors.verticalCenter: rpmGauge.verticalCenter
                anchors.rightMargin: 50
                font.family: "Helvetica"
                font.pixelSize: 12
                font.bold: Font.ExtraBold
                font.capitalization: Font.AllUppercase
                color: "white"
                text: "Housing"
            }
            Text {
                id: outlet
                anchors.verticalCenter: label_outlet.verticalCenter
                anchors.horizontalCenter: label_outlet.horizontalCenter
                anchors.top: label_outlet.bottom
                font.family: "Helvetica"
                font.pixelSize: 25
                font.bold: Font.ExtraBold
                font.capitalization: Font.AllUppercase
                color: "white"
                text: "50"
            }


            Text {
                id: label_intake
                anchors.left: rpmGauge.left
                anchors.verticalCenter: rpmGauge.verticalCenter
                anchors.leftMargin: 50
                font.family: "Helvetica"
                font.pixelSize: 12
                font.bold: Font.ExtraBold
                font.capitalization: Font.AllUppercase
                color: "white"
                text: "Intake"
            }
            Text {
                id: intake
                anchors.verticalCenter: label_intake.verticalCenter
                anchors.horizontalCenter: label_intake.horizontalCenter
                anchors.top: label_intake.bottom
                font.family: "Helvetica"
                font.pixelSize: 25
                font.bold: Font.ExtraBold
                font.capitalization: Font.AllUppercase
                color: "white"
                text: intakeTemp.temp
            }


         }


    }
}


