import QtQuick 2.4
import QtQuick.Controls 2.4
import DblpSearcher 1.0

Row {
    id: xmlLine

//    readonly property string tagColor: "#a04b6d"
//    readonly property string tagColor: "#7997c6"
    readonly property string tagColor: "#6ea0ef"
//    readonly property string attributesColor: "#c4c166"
    readonly property string attributesColor: "#e5e169"
    readonly property string contentColor: "#f7f7f7"
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

    Text {
        visible: type === DblpXmlLine.Inline
        font.pointSize: fontSize
        text: content
        wrapMode: Text.WordWrap
        textFormat: Text.RichText
        color: contentColor
    }

    Text {
        visible: type === DblpXmlLine.Inline
        font.pointSize: fontSize
        text: "</" + tag + ">"
        wrapMode: Text.WordWrap
        textFormat: Text.PlainText
        color: tagColor
    }
}
