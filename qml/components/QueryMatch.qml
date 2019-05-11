import QtQuick 2.4
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

Rectangle {
    height: l1.height + l2.height
    width: l1.width
    color: "red"

    ColumnLayout {
        id: innerLayout
        anchors.fill: parent
        Label {
            id: l1
            text: "Publication"
            Layout.fillWidth: true

        }
        Label {
            id: l2
            text: "Venue"
            Layout.fillWidth: true

        }
    }
}















