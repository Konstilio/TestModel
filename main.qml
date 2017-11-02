import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtTest 1.1
import "."

Rectangle {
    id: _main
    width: 700
    height: 700
    property var mainModel: BoardModel
    ListView{
        id: _lanesList
        width: _main.width
        height: _main.height
        anchors.top:_main.top
        model: _main.mainModel
        property int visualWidth: _main.mainModel.visualWidth

        delegate: Lane {
            id: _Lane

            width: Style.cardWidth * _lanesList.visualWidth + Style.addButtonWidth
            state: "expanded"
            model: columnsModel
            indexTitle: title
            modelIndex: index
            mainModel: _main.mainModel

            states: [
                State {
                    name: "expanded"
                    PropertyChanges { target: _Lane; height: columnsModel.visualHeight * Style.cardHeight + ((columnsModel.rowCount() > 0) + 1) * Style.nameOffset}
                },
                State {
                    name: "hidden"
                    PropertyChanges { target: _Lane; height: Style.nameOffset}
                }
            ]


            transitions: [
                Transition {
                    from: "*"; to: "expanded"
                    NumberAnimation { properties: "height"; easing.type: Easing.OutBounce; duration: 500 }
                },
                Transition {
                    from: "*"; to: "hidden"
                    NumberAnimation { properties: "height"; easing.type: Easing.InOutQuad; duration: 500 }
                }
            ]

            onExpandClicked: {
                console.log(_Lane.state)
                _Lane.state = _Lane.state === "expanded" ? "hidden" : "expanded"
                console.log(_Lane.state)
            }
        }
        Button {
            id: _addLane
            y: - _lanesList.contentY + _lanesList.contentHeight + Style.addButtonOffset
            height: Style.addButtonWidth
            width: Style.addButtonHeight
            onClicked: {
                _main.mainModel.appendLane()
            }
        }
    }
}
