import QtQuick 2.4
import QtQuick.Window 2.4
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.1

ApplicationWindow {
    property string publicationColor: "#f7eccd"
    property string venueColor: "#d5eded"
    property string publicationVenueColor: "#dcf4de"

    function humanTime(ms) {
        const MS = ms % 1000;

        if (ms < 1000)
            return MS + "ms";

        const S = parseInt((ms % 60000) / 1000);

        if (ms < 60000)
            return S + "s " + MS + "ms"

        const M = parseInt(ms / 60000);


        return M + "m " + S  + "s " + MS + "ms"
    }


    id: mainWindow
    visible: _main.visible
    minimumWidth: 600
    minimumHeight: 480
    width: 800
    height: 600

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

        Label {
            id: queryTime
            color: "#262a27"
            text: "Query time: " + humanTime(_main.queryTime)
            font.bold: true
            Layout.topMargin: 10
            visible: _main.status === "resolved"
            Layout.fillWidth: true
        }

        Label {
            id: matchesCount
            text: "Query matches: " + _main.matchesCount
            font.bold: true
            visible: _main.status === "resolved"
            Layout.fillWidth: true
        }

        Rectangle {
            id: matchesSeparator
            height: 1
            visible: _main.status === "resolved"
            color: "#80282828"
            Layout.bottomMargin: 10
            Layout.topMargin: 10
            Layout.fillWidth: true
        }

        Item {
            id: queryResultsContainer
            Layout.fillHeight: true
            Layout.fillWidth: true

            BusyIndicator {
                id: queryProgress
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                visible: _main.status === "resolving"
            }

            ListView {
                id: queryResults
                clip: true
                anchors.fill: parent
                visible: _main.status === "resolved"
                ScrollBar.vertical: ScrollBar {
                    active: true
                    policy: ScrollBar.AlwaysOn
                    minimumSize: 0.1
                }

                // == QUERY MATCH ==
                delegate: Rectangle {
                    id: queryMatchContainer
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
                                    font.italic: true
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
                                    color: "steelblue"
                                    Layout.fillWidth: true
                                    font.italic: true
                                    font.underline: true

                                    MouseArea {
                                        anchors.fill: parent
                                        cursorShape: Qt.OpenHandCursor
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

                model: _main.matches

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

    footer: Item {
        height: 28
        width: mainWindow.width

        Rectangle {
            id: rectangle1
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
                        text: "Publication+Venue"
                    }
                }
            }
        }

    }
}















































































/*##^## Designer {
    D{i:21;anchors_height:30}D{i:22;anchors_height:30}D{i:30;anchors_height:30}D{i:29;anchors_height:30}
D{i:28;anchors_height:30}
}
 ##^##*/
