import QtQuick 2.0

Rectangle {
    color: ( styleData.row % 2 == 0 ) ? "white" : "lightblue"
    height: 20

    Text {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left // by default x is set to 0 so this had no effect
        text: styleData.value
    }
}
