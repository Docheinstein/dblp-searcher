import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import DblpSearcher 1.0

ApplicationWindow {
    // Consts

    readonly property string publicationColor: "#f7eccd"
    readonly property string venueColor: "#d5eded"
    readonly property string publicationVenueColor: "#dcf4de"

    // Funcs

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

    function clearStack() {
        mainStackView.pop(null)
    }

    function doSearch() {
        clearStack()
        MainWindow.doSearch(mainSearchBar.text)
    }

    // Signals binding

    Connections {
        target: MainWindow
        onOpenElementDetailsRequired: {
//            console.log("Element details opening required for: " + serial)
//            console.log("Going to push:" + Qt.resolvedUrl("ElementDetails.qml"))
            mainStackView.push(
                "qrc:/qml/ElementDetails.qml",
                {
                    "elementSerial": serial
                }
            )
        }

        onPopViewRequired: {
//            console.log("Stackview pop required")
            mainStackView.pop()
        }
    }

    // View

    id: main
    visible: MainWindow.shown
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


            // SEARCH BAR
            TextField {
                id: mainSearchBar
                selectByMouse: true

                background: Rectangle {
                    radius: 10
                    implicitHeight: 40
                    border.color: "#d9d3d3"
                    border.width: 1
                }

                padding: 10
                horizontalAlignment: Text.AlignLeft
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                font.pixelSize: 14
                placeholderText: "Submit a query..."
                focus: true
                Keys.onReturnPressed: doSearch()

                // SEARCH BUTTON
                Button {
                    id: mainSearchButton
                    width: 50
                    // spacing: 5
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 0
                    anchors.top: parent.top
                    anchors.topMargin: 0
                    anchors.right: parent.right
                    anchors.rightMargin: 0

                    Image   {
                        anchors.fill: parent
                        anchors.bottomMargin: 5
                        anchors.topMargin: 5
                        anchors.rightMargin: 5
                        anchors.leftMargin: 5
                        source: "qrc:/img/search.png"
                        fillMode: Image.PreserveAspectFit
                    }

                    MouseArea {
                        hoverEnabled: true
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: doSearch()
                        onEntered: { mainSearchButton.state = "hovering" }
                        onExited: { mainSearchButton.state = "" }
                    }

                    background: Rectangle {
                        color: mainSearchButton.state === "hovering" ? "#2d627b" : "#4e92b2"
                        radius: 10
                        border.width: 0
                    }

//                    background: Rectangle {

//                    }
                }
            }
        }

        // MAIN STACK VIEW
        StackView {
            id: mainStackView
            Layout.fillWidth: true
            Layout.fillHeight: true
            initialItem: Loader {
                source: "qrc:/qml/QueryOutcome.qml"
            }
        }
    }

    // FOOTER

    Item {
        id: mainFooter
        height: 28
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 0

        Rectangle {
            id: mainFooterRectangle
            anchors.fill: parent
            color: "#d3d3d3"
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
                        border.color: "#80a2a2a2"
                        border.width: 1
                        radius: 2
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
                        border.color: "#80a2a2a2"
                        border.width: 1
                        radius: 4
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
                        border.color: "#80a2a2a2"
                        border.width: 1
                        radius: 4
                    }

                    Text {
                        text: "Publication & Venue"
                    }
                }

            }

        }
    }
}








