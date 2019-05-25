import QtQuick 2.0
import QtQuick.Controls 2.0
import DblpSearcher 1.0

Flow {
    id: xmlLine

    readonly property string tagColor: "#6ea0ef"
    readonly property string attributesColor: "#e5e169"
    readonly property string contentColor: "#f7f7f7"
    readonly property string linkColor: "steelblue"
    readonly property int fontSize: 10

    property QtObject parentRef; // Bound from outside

    width: parentRef.width

    Text {
        text:  (indent ? "    " : "") + "<" + tag
        font.pointSize: fontSize
        textFormat: Text.PlainText
        color: tagColor
    }

    Text {
        visible: attributes.length > 0
        text: " " + attributes
        font.pointSize: fontSize
        textFormat: Text.PlainText
        color: attributesColor
    }

    Text {
        text: ">"
        font.pointSize: fontSize
        textFormat: Text.PlainText
        color: tagColor
    }

    // TAG CONTENT
    Text {
        visible: type === DblpXmlLine.Inline
        font.pointSize: fontSize
        text: content
        textFormat: Text.RichText
        color: (crossref >= 0) ? "steelblue" : contentColor
        font.italic: (crossref >= 0)
        font.underline: (crossref >= 0)

        MouseArea {
            visible: (crossref >= 0)
            enabled: (crossref >= 0)
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: MainWindow.openElementDetails(crossref)
        }
    }
    // TAG CONTENT END

    Text {
        visible: type === DblpXmlLine.Inline
        font.pointSize: fontSize
        text: "</" + tag + ">"
        textFormat: Text.PlainText
        color: tagColor
    }
}
