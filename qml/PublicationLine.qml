import QtQuick 2.4
import QtQuick.Controls 2.4
import DblpSearcher 1.0

Row {
    id: xmlLine

    readonly property string linkColor: "#52BAE4"

    property QtObject parentRef; // Bound from outside

    width: parentRef.width

    Text {
        text:  identifier
        color: linkColor
        font.italic: true
        font.underline: true

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: MainWindow.openElementDetails(serial)
        }
    }
}
