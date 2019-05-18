import QtQuick 2.4
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import DblpSearcher 1.0

Rectangle {
    readonly property string publicationColor: "#f7eccd"
    readonly property string venueColor: "#d5eded"
    readonly property string publicationVenueColor: "#dcf4de"
    readonly property string linkColor: "steelblue"

    property QtObject parentRef; // Bound from outside

    id: queryMatch
    height: queryMatchLayout.implicitHeight
    width: parentRef.width
    color: {
        if (model.type === QueryMatchType.Publication) {
            publicationColor
        }
        else if (model.type === QueryMatchType.Venue ){
            venueColor
        }
        else if (model.type === QueryMatchType.PublicationVenue) {
            publicationVenueColor
        } else {
            "white"
        }
    }

    ColumnLayout {
        id: queryMatchLayout
        anchors.fill: parent

        RowLayout {
            Layout.leftMargin: 14
            Layout.rightMargin: 14
            Layout.bottomMargin: 8
            Layout.topMargin: 8
            Layout.fillHeight: true
            Layout.fillWidth: true

            spacing: 15

            ColumnLayout {
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                spacing: 5
                Layout.fillHeight: true

                Label {
                    id: queryMatchRank
                    text: model.rank + "."
                    font.pointSize: 10
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                Label {
                    id: queryMatchScore
                    color: "#595e61"
                    text: "(" + model.score.toFixed(2) + ")"
                    font.pointSize: 8
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }
            }

            ColumnLayout {
                id: queryMatchInnerLayout
                Layout.fillHeight: true
                Layout.fillWidth: true

                Label {
                    id: queryMatchPublicationType
                    visible: model.type === QueryMatchType.Publication ||
                             model.type === QueryMatchType.PublicationVenue
                    text: model.publicationElementType
                    Layout.fillWidth: true
                    font.pointSize: 10
                    font.bold: true
                }

                Label {
                    id: queryMatchPublicationIdentifier
                    visible: model.type === QueryMatchType.Publication ||
                             model.type === QueryMatchType.PublicationVenue
                    text: model.publicationIdentifier
                    Layout.fillWidth: true
                    color: linkColor
                    font.italic: true
                    font.underline: true

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: MainWindow.openElementDetails(model.publicationSerial)
                    }
                }

                Label {
                    id: queryMatchVenueType
                    visible: model.type === QueryMatchType.Venue ||
                             model.type === QueryMatchType.PublicationVenue
                    text: model.venueElementType
                    Layout.fillWidth: true
                    font.pointSize: 10
                    font.bold: true
                }

                Label {
                    id: queryMatchVenueIdentifier
                    visible: model.type === QueryMatchType.Venue ||
                             model.type === QueryMatchType.PublicationVenue
                    text: model.venueIdentifier
                    Layout.fillWidth: true
                    color: linkColor
                    font.italic: true
                    font.underline: true

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: MainWindow.openElementDetails(model.venueSerial)
                    }
                }

            }
        }

        Rectangle  {
            id: queryMatchDivider
            height: 1
            color: "#4c292929"
            Layout.fillWidth: true
        }
    }
}
