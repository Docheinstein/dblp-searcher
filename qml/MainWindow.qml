import QtQuick 2.4
import QtQuick.Window 2.4
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.1
import DblpSearcher 1.0

ApplicationWindow {
    readonly property string publicationColor: "#f7eccd"
    readonly property string venueColor: "#d5eded"
    readonly property string publicationVenueColor: "#dcf4de"

    function humanTime(ms) {
        const MS = ms % 1000;

        if (ms < 1000)
            return MS + "ms";

        const S = parseInt((ms % 60000) / 1000);
        const SS = S + "." + parseInt((MS / 10)) + "s";

        if (ms < 60000)
            return SS;

        const M = parseInt(ms / 60000);

        return M + "m " + SS;
    }

    function doSearch() {
        mainStackView.pop(null)
        QueryResolver.doSearch(mainSearchBar.text)
    }

    id: main
    visible: true
    minimumWidth: 600
    minimumHeight: 480
    width: 800
    height: 600

    ColumnLayout {
        id: mainColumnLayout
        anchors.rightMargin: 30
        anchors.leftMargin: 30
        anchors.bottomMargin: 30
        anchors.topMargin: 30
        spacing: 5
        anchors.fill: parent

        RowLayout {
            id: mainRowLayout
            spacing: 10
            Layout.fillHeight: false
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop

            // LOGO
            Image {
                id: mainDblpLogo
                visible: false
                sourceSize.height: 40
                fillMode: Image.PreserveAspectFit
                source: ""
            }

            // SEARCH BAR
            TextField {
                id: mainSearchBar
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
                Keys.onReturnPressed: doSearch()

                // SEARCH BUTTON
                Button {
                    id: mainSearchButton
                    width: 50
                    spacing: 5
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 0
                    anchors.top: parent.top
                    anchors.topMargin: 0
                    anchors.right: parent.right
                    anchors.rightMargin: 0

                    icon.source: "qrc:/img/search.png"
                    icon.color: "white"

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: doSearch()
                    }

                    background: Rectangle {
                        color: mainSearchButton.hovered ? "#2d627b" : "#bf2d627b"
                        radius: 10
                        border.width: 0
                    }
                }
            }
        }

        // MAIN STACK VIEW
        StackView {
            id: mainStackView
            Layout.fillWidth: true
            Layout.fillHeight: true
            initialItem: Loader {
                source: "QueryOutcome.qml"
            }
        }
    }

    // FOOTER

    footer: Item {
        id: mainFooter
        width: 800
        height: 28

        Rectangle {
            id: mainFooterRectangle
            anchors.fill: parent
            color: "#e5e8e7"
            border.color: "#80a2a2a2"
            border.width: 1

            Row {
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.rightMargin: 10

                spacing: 16

                RowLayout {
                    height: 16
                    spacing: 4

                    Rectangle {
                        height: 16
                        width: 16
                        color: publicationColor
                    }

                    Text {
                        text: "Publication"
                    }
                }

                RowLayout {
                    height: 16
                    spacing: 4

                    Rectangle {
                        height: 16
                        width: 16
                        color: venueColor
                    }

                    Text {
                        text: "Venue"
                    }
                }
                RowLayout {
                    height: 16
                    spacing: 4

                    Rectangle {
                        height: 16
                        width: 16
                        color: publicationVenueColor
                    }

                    Text {
                        text: "Publication & Venue"
                    }
                }
            }
        }
    }
}

/*##^## Designer {
    D{i:18;anchors_height:30}D{i:19;anchors_height:30}D{i:20;anchors_height:30}D{i:11;anchors_width:800}
}
 ##^##*/
