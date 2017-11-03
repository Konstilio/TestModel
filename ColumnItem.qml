import QtQuick 2.0
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.4
import "."

Item {
    id: _column
    property var model
    property var indexTitle

    Rectangle {
        id: _columnRect
        antialiasing: true
        anchors {
            topMargin: 1
            leftMargin: 1
            rightMargin: 1
            bottomMargin: 1
            fill: parent
        }

        color:"yellow"
        border.color: "black"
        border.width: 1
        Text {
            id: _columnName
            text: indexTitle
            anchors.top: parent.top
            height: _columnRect.height > 10 ? Style.nameOffset : 0 // What ???
            visible: height > 0
            color: "blue"
        }
        Button {
            id: _addCard
            anchors{
                top: _columnName.top
            }
            height: Style.nameOffset
            x: _columnName.x + _columnName.paintedWidth
            onClicked: {
                _column.model.append()
            }
        }

        ListView
        {
            id: _cardsList
            anchors.top: _columnName.bottom
            height: _column.height - _columnName.height
            width: parent.width
            model: _column.model
            delegate: Card {
                indexTitle: title
                width: _cardsList.width
                height: _cardsList.height / 3
            }
        }
    }

    DropShadow {
        anchors.fill: _columnRect
        cached: true
        horizontalOffset: 3
        verticalOffset: 3
        radius: 5.0
        samples: 16
        color: "#80000000"
        source: _columnRect
    }

    onHeightChanged: {
        console.log("Column " + _column.height)
        console.log(_columnName.height)
    }
}
