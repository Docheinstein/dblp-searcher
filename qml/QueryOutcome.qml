import QtQuick 2.4
import QtQuick.Controls 2.3
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
            ScrollBar.vertical: ScrollBar {
                active: true
                policy: ScrollBar.AlwaysOn
                minimumSize: 0.1
            }

            // == QUERY MATCH ==
            delegate: Loader {
                id: queryMatchLoader
                source: "QueryMatch.qml"

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
