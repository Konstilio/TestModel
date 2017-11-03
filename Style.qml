pragma Singleton
import QtQuick 2.0
QtObject {
    readonly property int cardWidth: 200
    readonly property int cardHeight: 100
    readonly property int nameOffset: 30

    readonly property int addButtonWidth: 25
    readonly property int addButtonHeight: 25
    readonly property int addButtonOffset: 2

    readonly property var sEXPANDED: "expanded"
    readonly property var sCOLLAPSED: "collapsed"
}
