import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 2.0
import DblpSearcher 1.0

Window {
    id: splash
    color: "transparent"
    modality: Qt.ApplicationModal
    flags: Qt.SplashScreen
    visible: SplashWindow.shown

    x: (Screen.width - splash.width) / 2
    y: (Screen.height - splash.height) / 2
    width: 480
    height: 360

    title: "Dblp Searcher"

//    Connections {
//        target: SplashWindow
//        onShownChanged: {
//            console.log("onShown: " + SplashWindow.shown)
//        }
//    }

    Image {
        id: splashBackground
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        source: "qrc:/img/splash.png"
    }

    Column {
        id: splashLayout
        spacing: 20
        anchors.top: parent.top
        anchors.topMargin: splash.height / 2 + 30
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0

        Text {
            id: splashText
            color: "#363636"
            text: SplashWindow.status
            font.pointSize: 11
            renderType: Text.NativeRendering
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.right: parent.right
            anchors.rightMargin: 0
            horizontalAlignment: Text.AlignHCenter
        }

        ProgressBar {
            id: splashProgress
            anchors.horizontalCenter: parent.horizontalCenter
            value: SplashWindow.progress
        }
    }
}















