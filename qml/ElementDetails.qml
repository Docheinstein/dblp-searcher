import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import DblpSearcher 1.0

ColumnLayout {

    readonly property string tabSelectedColor: "#2d627b"
    readonly property string tabUnselectedColor: "#d7dfed"
    readonly property string tabSelectedTextColor: "#f9f9f9"
    readonly property string tabUnselectedTextColor: "#27282a"
    readonly property string xmlBackgroundColor: "#353742"

    // Bound from outside
    property int elementSerial;

    spacing: 0

    id: elementDetails

    ElementDetails {
        id: elementDetailsModel
        serial: elementSerial // Create with the bound serial

        onHasXmlChanged: {
//            console.log('Has xml changed: ' + hasXml)
            const tabCount = elementDetailsTabBar.count
            if (hasXml) {
                elementDetailsTabBar.addItem(
                            elementDetailsXmlTabComponent
                            .createObject(elementDetailsTabBar,
                                          {"index": tabCount})
                );
            }
        }

        onHasPublicationsChanged: {
//            console.log('Has publications changed: ' + hasPublications)
            const tabCount = elementDetailsTabBar.count
            if (hasPublications) {
                elementDetailsTabBar.addItem(
                            elementDetailsPublicationsTabComponent
                            .createObject(elementDetailsTabBar,
                                          {"index": tabCount})
                );
            }
        }
    }

    Button {
        id: elementDetailsBackButton

        width: 60
        Layout.topMargin: 8

        icon.source: "qrc:/img/back.png"
        icon.color: "#3e4649"

        background: Rectangle {
            // In order to have a transparent background
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: MainWindow.popView()
        }
    }

    Label {
        id: elementDetailsIdentifier
        text: elementDetailsModel.identifier
        Layout.topMargin: 8
        verticalAlignment: Text.AlignVCenter
        font.bold: true
        font.pointSize: 11
        Layout.fillWidth: true
        horizontalAlignment: Text.AlignHCenter
    }

    TabBar {
        id: elementDetailsTabBar

        width: parent.width
        Layout.topMargin: 8
        height: 40
        Layout.fillWidth: true


        Component {
            id: elementDetailsXmlTabComponent

            TabButton {
                readonly property bool selected: elementDetailsTabBar.currentIndex
                                                 === index

                property int index;

                id: elementDetailsXmlTab
                text: "XML"

                background: Rectangle {
                    color: elementDetailsXmlTab.selected ?
                               tabSelectedColor : tabUnselectedColor
                }

                contentItem: Text {
                    color: elementDetailsXmlTab.selected ?
                               tabSelectedTextColor : tabUnselectedTextColor
                    text: elementDetailsXmlTab.text
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }
        Component {
            id: elementDetailsPublicationsTabComponent

            TabButton {
                readonly property bool selected: elementDetailsTabBar.currentIndex
                                                 === index

                property int index;

                id: elementDetailsPublicationsTab
                text: "Publications"

                background: Rectangle {
                    color: elementDetailsPublicationsTab.selected ?
                               tabSelectedColor : tabUnselectedColor
                }
                contentItem: Text {
                    color: elementDetailsPublicationsTab.selected ?
                               tabSelectedTextColor : tabUnselectedTextColor
                    text: elementDetailsPublicationsTab.text
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }
    }

    StackLayout {
        id: elementDetailsStack

        Layout.fillHeight: true
        Layout.fillWidth: true

        currentIndex: elementDetailsTabBar.currentIndex

        Item {
            id: elementDetailsXmlView
            Layout.bottomMargin: 10
            Layout.fillHeight: true
            Layout.fillWidth: true
            visible: elementDetailsModel.hasXml

            Rectangle {
                width: parent.width
                height: parent.height
                color: xmlBackgroundColor
                visible: elementDetailsModel.hasXml
            }

            ListView {
                id: elementDetailsXmlListView
                anchors.rightMargin: 12
                anchors.leftMargin: 12
                anchors.topMargin: 12
                anchors.fill: parent

                ScrollBar.vertical: ScrollBar {
                    active: true
                    minimumSize: 0.1
                    policy: ScrollBar.AsNeeded
                }

                clip: true

                delegate: Loader {
                    id: elementDetailsXmlLineLoader
                    source: "DblpXmlLine.qml"

                    Binding {
                        target: elementDetailsXmlLineLoader.item
                        property: "parentRef"
                        value: elementDetailsXmlListView
                    }
                }

                model: elementDetailsModel.xmlLines
            }
        }

        Item {
            id: elementDetailsPublicationsView
            visible: elementDetailsModel.publications

            Layout.bottomMargin: 10
            Layout.fillHeight: true
            Layout.fillWidth: true

            ColumnLayout {
                anchors.fill: parent

                Text {
                    text: "Publications: " + elementDetailsModel.publicationsCount
                    font.bold: true
                    Layout.topMargin: 12
                    Layout.rightMargin: 12
                    Layout.leftMargin: 12
                    Layout.fillWidth: true
                }

                ListView {
                    id: elementDetailsPublicationsListView
                    Layout.leftMargin: 12
                    Layout.rightMargin: 12
                    Layout.topMargin: 12
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    height: 20

                    ScrollBar.vertical: ScrollBar {
                        active: true
                        minimumSize: 0.1
                        policy: ScrollBar.AsNeeded
                    }

                    clip: true

                    delegate: Loader {
                        id: elementDetailsPublicationLineLoader
                        source: "PublicationLine.qml"

                        Binding {
                            target: elementDetailsPublicationLineLoader.item
                            property: "parentRef"
                            value: elementDetailsPublicationsListView
                        }
                    }

                    model: elementDetailsModel.publications
                }
            }
        }
    }
}
