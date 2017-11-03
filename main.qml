import QtQuick 2.0
import QtQuick.Controls 1.4
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
            state: Style.sEXPANDED
            model: proxyModel
            indexTitle: title
            modelIndex: index

            states: [
                State {
                    name: Style.sEXPANDED
                    PropertyChanges {
                        target: _Lane;
                        height: proxyModel.visualHeight * Style.cardHeight + ((proxyModel.rowCount() > 0) + 1) * Style.nameOffset
                    }
                },
                State {
                    name: Style.sCOLLAPSED
                    PropertyChanges {
                        target: _Lane;
                        height: Style.nameOffset
                    }
                }
            ]


            transitions: [
                Transition {
                    from: "*"; to: Style.sEXPANDED
                    NumberAnimation { properties: "height"; easing.type: Easing.OutBounce; duration: 500 }
                },
                Transition {
                    from: "*"; to: Style.sCOLLAPSED
                    NumberAnimation { properties: "height"; easing.type: Easing.InOutQuad; duration: 500 }
                }
            ]

            onExpandClicked: {
                console.log(_Lane.state)
                _Lane.state = _Lane.state === Style.sEXPANDED ? Style.sCOLLAPSED : Style.sEXPANDED
                console.log(_Lane.state)
            }
        }
        Button {
            id: _addLane
            y: - _lanesList.contentY + _lanesList.contentHeight + Style.addButtonOffset
            height: Style.addButtonWidth
            width: Style.addButtonHeight
            onClicked: {
                _main.mainModel.append()
            }
        }
    }
}
