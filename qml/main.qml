import QtQuick 2.4
import QtQuick.Window 2.4
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.1

ApplicationWindow {
    id: mainWindow
    visible: _main.visible
    minimumWidth: 480
    minimumHeight: 360

    ColumnLayout {
        id: columnLayout
        anchors.rightMargin: 30
        anchors.leftMargin: 30
        anchors.bottomMargin: 30
        anchors.topMargin: 30
        spacing: 5
        anchors.fill: parent

        RowLayout {
            id: rowLayout
            spacing: 10
            Layout.fillHeight: false
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop

            Image {
                id: dblpLogo
                visible: false
                sourceSize.height: 40
                fillMode: Image.PreserveAspectFit
                source: "qrc:/img/dblp-logo.png"
            }

            TextField {
                id: searchBar
                selectByMouse: true
                rightPadding: 60
                background: Rectangle {
                    implicitHeight: 40
                    border.color: "#d9d3d3"
                    border.width: 1
                    radius: 10
                }

                Layout.rightMargin: 0
                horizontalAlignment: Text.AlignLeft
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                font.pixelSize: 12
                placeholderText: "Submit a query..."

                Button {
                    id: searchButton
                    width: 50
                    spacing: 6
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 0
                    anchors.top: parent.top
                    anchors.topMargin: 0
                    anchors.right: parent.right
                    anchors.rightMargin: 0
//                    Layout.fillHeight: true
//                    Layout.fillWidth: false

                    icon.source: "qrc:/img/search.png"
                    icon.color: "white"

                    onClicked: _main.doSearch(searchBar.text)

                    background: Rectangle {
                        color: searchButton.hovered ? "#2d627b" : "#bf2d627b"
                        radius: 10
                        border.width: 0
                    }
                }
            }


        }
    }

    TextEdit {
        id: textEdit
        x: 144
        y: 154
        width: 80
        height: 20
        text: qsTr("Text Edit")
        font.pixelSize: 12
    }

    TextInput {
        id: textInput
        x: 149
        y: 230
        width: 80
        height: 20
        text: qsTr("Text Input")
        font.pixelSize: 12
    }

    TextArea {
        id: textArea
        x: 161
        y: 307
        text: qsTr("Text Area")
    }
}

























































































/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
