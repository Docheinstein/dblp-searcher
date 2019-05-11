import QtQuick 2.4
import QtQuick.Window 2.4
import QtQuick.Controls 2.4

Window {
    id: splashWindow
    color: "transparent"
    modality: Qt.ApplicationModal
    flags: Qt.SplashScreen
    visible: _splash.visible

    //    x: (Screen.width - splash.width) / 2
    //    y: (Screen.height - splash.height) / 2
    width: 480
    height: 360
    title: "Dblp Searcher"

    Image {
        id: splashImage
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        source: "qrc:/img/splash.png"
    }

    Column {
        id: splashLayout
        spacing: 20
        anchors.top: parent.top
        anchors.topMargin: splashWindow.height / 2 + 30
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0

        Text {
            id: splashText
            color: "#363636"
            text: _splash.status
            font.pointSize: 11
            renderType: Text.NativeRendering
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.right: parent.right
            anchors.rightMargin: 0
            horizontalAlignment: Text.AlignHCenter
        }

        ProgressBar {
            id: progressBar
            anchors.horizontalCenter: parent.horizontalCenter
            value: _splash.progress
        }

    }

//    Component.onCompleted: visible = true
}














