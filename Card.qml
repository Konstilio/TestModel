import QtQuick 2.0

Item {
    id: _card
    property var indexTitle

    Rectangle {
        antialiasing: true
        anchors {
            topMargin: 0
            leftMargin: 2
            rightMargin: 2
            bottomMargin: 3
            fill: parent
        }

        color:"#FA0000"
        border.color: "black"
        border.width: 1
        Text
        {
            id: _cardName
            anchors.fill: parent
            text: indexTitle
        }
    }
}
