import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import DblpSearcher 1.0

ColumnLayout {
    id: queryOutcome
    anchors.fill: parent
    visible: true

    Label {
        id: queryOutcomeQueryTime
        color: "#262a27"
        text: "Query time: " + humanTime(MainWindow.queryTime)
        font.bold: true
        Layout.topMargin: 10
        visible: MainWindow.queryStatus === MainWindow.Done
        Layout.fillWidth: true
    }

    Label {
        id: queryOutcomeMatchesCount
        text: "Query matches: " + MainWindow.matchesCount
        font.bold: true
        visible: MainWindow.queryStatus === MainWindow.Done
        Layout.fillWidth: true
    }

    Rectangle {
        id: queryOutcomeMatchesSeparator
        height: 1
        visible: MainWindow.queryStatus === MainWindow.Done
        color: "#80282828"
        Layout.bottomMargin: 10
        Layout.topMargin: 10
        Layout.fillWidth: true
    }
//    ColumnLayout {

//        Label {
//            text: "CH" + queryOutcomeResults.contentHeight
//            Layout.fillWidth: true

//        }
//        Label {
//            text: "CY" + queryOutcomeResults.contentY
//            Layout.fillWidth: true

//        }
//        Label {
//            text: "H" + queryOutcomeResults.height
//            Layout.fillWidth: true

//        }
//        Label {
//            text: "IH" + queryOutcomeResults.implicitHeight
//            Layout.fillWidth: true

//        }
//        Label {
//            text: "Y" + queryOutcomeResults.y
//            Layout.fillWidth: true

//        }
//    }
    Item {
        id: queryOutcomeResultsContainer
        Layout.fillHeight: true
        Layout.fillWidth: true

        BusyIndicator {
            id: queryOutcomeQueryProgress
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            visible: MainWindow.queryStatus === MainWindow.Resolving
        }

        ListView {
            id: queryOutcomeResults
            clip: true
            anchors.fill: parent
            visible: MainWindow.queryStatus === MainWindow.Done
//             contentY: queryOutcomeResultsScroll.position * contentHeight
            // Keys.onUpPressed: queryOutcomeResultsScroll.increase()
            // Keys.onDownPressed: queryOutcomeResultsScroll.decrease()

            // Custom scrollbar (for support Qt 5.7 and having a min size)
//            Item {
//                anchors.right: parent.right
//                anchors.top: parent.top
//                anchors.bottom: parent.bottom
//                width: 8
//                anchors.bottomMargin: 0
//                anchors.rightMargin: 0
//                anchors.topMargin: 0

//                Rectangle {
//                    property int indicatorSize: 60
//                    x: 0
//                    y: (parent.height - indicatorSize) * queryOutcomeResults.contentY /
//                       (queryOutcomeResults.contentHeight - queryOutcomeResults.height)
//                    anchors.right: parent.right
//                    anchors.rightMargin: 0
//                    color: "#606060"
//                    width: parent.width
//                    height: indicatorSize
//                    radius: 6
//                }
//            }

            ScrollBar.vertical: ScrollBar {
                visible: true
                active: true
                orientation: Qt.Vertical
            }

//            ScrollBar {
//                readonly property real indicatorSize: 0.25
//                id: queryOutcomeResultsScroll
//                anchors.right: queryOutcomeResults.right
//                anchors.top: queryOutcomeResults.top
//                anchors.bottom: queryOutcomeResults.bottom
//                visible: true
//                hoverEnabled: true
//                active: true
//                size: indicatorSize
//                position: (queryOutcomeResults.contentY /
//                          (queryOutcomeResults.contentHeight - queryOutcomeResults.height))
//                          * (1 - indicatorSize)
//                orientation: Qt.Vertical

//                contentItem: Rectangle {
//                        implicitWidth: 6
//                        implicitHeight: 100
//                        radius: width / 2
//                        color: control.pressed ? "#81e889" : "#c2f4c6"
//                    }
//            }

            // == QUERY MATCH ==
            delegate: Loader {
                id: queryMatchLoader
                source: "qrc:/qml/QueryMatch.qml"

                Binding {
                    target: queryMatchLoader.item
                    property: "parentRef"
                    value: queryOutcomeResults
                }
            }

            model: MainWindow.matches
        }
    }

}
