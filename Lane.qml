import QtQuick 2.0
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.4
import "."

Item {
    id: _lane
    property var model
    property var indexTitle
    property var modelIndex // index of lane in main model


    signal expandClicked

    Rectangle{
        id: _laneRect
        antialiasing: true
        anchors {
            margins: 2
            fill: parent
        }

        color:"lightblue"
        border.color: "black"
        border.width: 1
        Text
        {
            id: _laneName
            anchors.top: parent.top
            text: indexTitle
            height: Style.nameOffset
        }
        ListView
        {
            id: _columnsList
            anchors.top: _laneName.bottom
            height: _laneRect.height - _laneName.height
            width: _laneRect.width
            model: _lane.model
            orientation: Qt.Horizontal
            property int columnsCount: _lane.model.rowCount()
            state: "expanded"
            delegate: ColumnItem {
                indexTitle: title
                model: proxyModel
                width: Style.cardWidth
                height: _columnsList.height
            }

        }
        Button {
            id: _addColumn
            anchors{
                right: _laneRect.right
                top: _laneRect.top
            }
            height: Style.nameOffset
            width: Style.addButtonWidth
            onClicked: {
                _lane.model.append()
            }
        }

        Button {
            id: _expandList
            anchors{
                right: _addColumn.left
                top: _laneRect.top
                rightMargin: 5
            }
            height: Style.nameOffset
            width: Style.addButtonWidth
            onClicked: {
                console.log("1")
                _lane.expandClicked();
            }
        }
    }

    DropShadow {
        anchors.fill: _laneRect
        cached: true
        horizontalOffset: 3
        verticalOffset: 3
        radius: 5.0
        samples: 16
        color: "#80000000"
        source: _laneRect
    }
}
