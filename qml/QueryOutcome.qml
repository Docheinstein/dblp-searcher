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
        text: "Query time: " + humanTime(QueryResolver.queryTime)
        font.bold: true
        Layout.topMargin: 10
        visible: QueryResolver.status === QueryResolver.Done
        Layout.fillWidth: true
    }

    Label {
        id: queryOutcomeMatchesCount
        text: "Query matches: " + QueryResolver.matchesCount
        font.bold: true
        visible: QueryResolver.status === QueryResolver.Done
        Layout.fillWidth: true
    }

    Rectangle {
        id: queryOutcomeMatchesSeparator
        height: 1
        visible: QueryResolver.status === QueryResolver.Done
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
            visible: QueryResolver.status === QueryResolver.Resolving
        }

        ListView {
            id: queryOutcomeResults
            clip: true
            anchors.fill: parent
            visible: QueryResolver.status === QueryResolver.Done
            ScrollBar.vertical: ScrollBar {
                active: true
                policy: ScrollBar.AlwaysOn
                minimumSize: 0.1
            }

            // == QUERY MATCH ==
            delegate: Loader {
                source: "QueryMatch.qml"
            }

            model: QueryResolver.matches

//                model: ListModel {
//                    ListElement {
//                        rank: 1
//                        type: "publication"
//                        publicationIdentifier: "/key/bdt/2019"
//                        publicationElementType: "Article"
//                        venueIdentifier: "/key/bdt/2019"
//                        venueElementType: "Journal"
//                        score: 0.4
//                    }
//                    ListElement {
//                        rank: 2
//                        type: "venue"
//                        publicationIdentifier: "/key/bdt/2019"
//                        publicationElementType: "Article"
//                        venueIdentifier: "/key/bdt/2019"
//                        venueElementType: "Journal"
//                        score: 0.2
//                    }
//                    ListElement {
//                        rank: 3
//                        type: "publication_venue"
//                        publicationIdentifier: "/key/bdt/2019"
//                        publicationElementType: "Article"
//                        venueIdentifier: "/key/bdt/2019"
//                        venueElementType: "Journal"
//                        score: 0.1
            //                    }
            //                }

        }
    }

}
