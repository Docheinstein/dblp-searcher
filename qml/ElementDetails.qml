import QtQuick 2.4
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import DblpSearcher 1.0

ColumnLayout {

    readonly property string tabSelectedColor: "#2d627b"
    readonly property string tabUnselectedColor: "#d7dfed"
    readonly property string tabSelectedTextColor: "#f9f9f9"
    readonly property string tabUnselectedTextColor: "#27282a"

    property int elementSerial;

    spacing: 0
    // Bound from outside

    id: elementDetails

    ElementDetails {
        id: elementDetailsModel
        serial: elementSerial // Create with the bound serial
    }

    Button {
        id: elementDetailsBackButton

        width: 60
        Layout.bottomMargin: 8
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

    TabBar {
        id: elementDetailsTabBar

        height: 40
        Layout.fillWidth: true

        TabButton {
            id: elementDetailsXmlTab
            text: "XML"

            readonly property bool selected: elementDetailsTabBar.currentIndex === 0

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
        TabButton {
            id: elementDetailsVenuesTab
            text: "Publications"

            readonly property bool selected: elementDetailsTabBar.currentIndex === 1

            background: Rectangle {
                color: elementDetailsVenuesTab.selected ?
                           tabSelectedColor : tabUnselectedColor
            }
            contentItem: Text {
                color: elementDetailsVenuesTab.selected ?
                           tabSelectedTextColor : tabUnselectedTextColor
                text: elementDetailsVenuesTab.text
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    StackLayout {
        Layout.rightMargin: 5
        Layout.leftMargin: 5
        Layout.topMargin: 20
        Layout.fillHeight: true
        Layout.fillWidth: true

//        currentIndex: elementDetailsTabBar.currentIndex
        currentIndex: 0

        Item {
            id: elementDetailsXmlView
            Layout.fillHeight: true
            Layout.fillWidth: true

            ListView {
                id: elementDetailsXmlListView
                anchors.fill: parent
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

//                model: ListModel {
//                    ListElement {
//                        tag: "article"
//                        type: "open"
//                        attributes: "key=\"journals/it/ele-eco\" mdate=\"2018-03-26\""
//                        indent: false
//                    }
//                    ListElement {
//                        tag: "author"
//                        type: "inline"
//                        attributes: ""
//                        content: "Amarjot <b>Singh</b>";
//                        indent: true
//                    }
//                    ListElement {
//                        tag: "article"
//                        type: "close"
//                        attributes: ""
//                        indent: false
//                    }
//                }
            }
        }

        Item {
            id: elementDetailsPublicationsView
        }
    }
}
