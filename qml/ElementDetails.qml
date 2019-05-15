import QtQuick 2.4
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import DblpSearcher 1.0

ColumnLayout {

    readonly property string tabSelectedColor: "#2d627b"
    readonly property string tabUnselectedColor: "#d7dfed"
    readonly property string tabSelectedTextColor: "#f9f9f9"
    readonly property string tabUnselectedTextColor: "#27282a"

    property int serial; // Bound from outside

    id: elementDetails

    ElementDetails {
        elementSerial: serial
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
        Layout.fillHeight: true
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
}
