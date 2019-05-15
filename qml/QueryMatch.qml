import QtQuick 2.4
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

Rectangle {
    readonly property string publicationColor: "#f7eccd"
    readonly property string venueColor: "#d5eded"
    readonly property string publicationVenueColor: "#dcf4de"

    function openElementDetails() {
//       stackView.push("ElementDetails.qml")
    }

    id: queryMatch
    height: queryMatchLayout.implicitHeight
    width: queryResults.width
    color: {
        if (model.type === "publication") {
            publicationColor
        }
        else if (model.type === "venue" ){
            venueColor
        }
        else if (model.type === "publication_venue") {
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
                    id: publicationType
                    visible: model.type === "publication" ||
                             model.type === "publication_venue"
                    text: model.publicationElementType
                    Layout.fillWidth: true
                    font.pointSize: 10
                    font.bold: true
                }

                Label {
                    id: publicationIdentifier
                    visible: model.type === "publication" ||
                             model.type === "publication_venue"
                    text: model.publicationIdentifier
                    Layout.fillWidth: true
                    color: "steelblue"
                    font.italic: true
                    font.underline: true

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: openElementDetails()
                    }
                }

                Label {
                    id: venueType
                    visible: model.type === "venue" ||
                             model.type === "publication_venue"
                    text: model.venueElementType
                    Layout.fillWidth: true
                    font.pointSize: 10
                    font.bold: true
                }

                Label {
                    id: venueIdentifier
                    visible: model.type === "venue" ||
                             model.type === "publication_venue"
                    text: model.venueIdentifier
                    Layout.fillWidth: true
                    color: "steelblue"
                    font.italic: true
                    font.underline: true

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: openElementDetails()
                    }
                }

            }
        }

        Rectangle  {
            id: rectangle
            height: 1
            color: "#4c292929"
            Layout.fillWidth: true
        }
    }
}
