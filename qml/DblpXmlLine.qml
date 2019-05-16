import QtQuick 2.4
import QtQuick.Controls 2.4
import DblpSearcher 1.0

Row {
    id: xmlLine

    property QtObject parentRef; // Bound from outside

    width: parentRef.width

    Text {
        text: (indent ? "    " : "") + "<" + tag + " " + attributes + ">"
        textFormat: Text.PlainText
    }

    Text {
        visible: type === DblpXmlLine.Inline
        text: content
        textFormat: Text.RichText
    }

    Text {
        visible: type === DblpXmlLine.Inline
        text: "</" + tag + ">"
        textFormat: Text.PlainText
    }
}
