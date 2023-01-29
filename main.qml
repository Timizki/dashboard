import QtQuick 2.15
import QtQuick.Window 2.15


Window {
    width: 1280
    height: 400
    visible: true
    title: qsTr("Dashboard")    

    AnimatedImage {

        id: outboard
        source: "images/outboard.png"
        height: 300
        width: 250
        x: 100
        y: 50
        fillMode: Image.PreserveAspectFit
        transformOrigin: Item.TopRight
        focus: true
        Keys.onLeftPressed: {
            down_trim.stop()
            up_trim.start()


        }
        Keys.onRightPressed:
            down_trim.start()
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

}
