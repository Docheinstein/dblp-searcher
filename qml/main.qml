import QtQuick 2.4
import QtQuick.Window 2.4
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.1

ApplicationWindow {
    property string _STATUS_NONE : "none";
    property string _STATUS_RESOLVING: "resolving";
    property string _STATUS_RESOLVED: "resolved";

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
                focus: true
                Keys.onReturnPressed: _main.doSearch(searchBar.text)

                Button {
                    id: searchButton
                    width: 50
                    spacing: 5
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

        Pane {
            id: pane
            width: 200
            height: 200
            Layout.fillHeight: true
            Layout.fillWidth: true

            BusyIndicator {
                id: busyIndicator
                width: 80
                height: 80
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                visible: _main.status === _STATUS_RESOLVING
            }
        }
    }
}

/*
constexpr static const char * const STATUS_NONE = "none";
constexpr static const char * const STATUS_RESOLVING = "resolving";
constexpr static const char * const STATUS_RESOLVED = "resolved";
*/



























































































/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
