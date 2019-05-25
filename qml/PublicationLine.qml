import QtQuick 2.0
import QtQuick.Controls 2.0
import DblpSearcher 1.0

Row {
    id: xmlLine

    property QtObject parentRef; // Bound from outside

    width: parentRef.width

    Text {
        text: rank + ". "
    }

    Text {
        color: "steelblue"
        text: identifier
        font.italic: true
        font.underline: true

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: MainWindow.openElementDetails(serial)
        }
    }
}
