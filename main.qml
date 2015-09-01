import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import engine 1.0

ApplicationWindow {
    property int lastIndex: -1
    property bool ifClicked: false
    id: mainWindow
    width: 700
    height: 700
    visible: true
    color: "#133843"

    Rectangle {
        width: 600
        height: 600
        color: "transparent"
        anchors.centerIn: parent

        Component {
            id: delegate

            Image {
                id: rect
                source: path
                height: board.cellHeight - board.cell/5
                width: board.cellWidth - board.cell/5
                visible: index < (modelSize - list.columnsCount) ? true : false

//                SequentialAnimation {
//                    running: mainWindow.lastIndex === index

//                    NumberAnimation {
//                        target: rect;
//                        duration: 40;
//                        property: "rotation";
//                        to: -60;
//                    }
//                    NumberAnimation {
//                        target: rect;
//                        duration: 40;
//                        property: "rotation";
//                        to: 60;
//                    }
//                    NumberAnimation {
//                        target: rect;
//                        duration: 40;
//                        property: "rotation";
//                        to: 0;
//                    }
//                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        list.swap(index);
                        lastIndex = index

                        if(list.movesCount >= list.maxMoves) {
                            if(list.scoreCount >= list.minScore) {
                                messageDialog.isVisible = true
                                messageDialog.isVictory = true
                            }
                            else {
                                messageDialog.isVisible = true
                                messageDialog.isVictory = false
                            }
                        }
                    }
                }
            }
        }

        GridView {
            property var cell: list.rowsCount > list.columnsCount ? list.rowsCount : list.columnsCount

            id:board
            anchors.centerIn: parent
            height: list.rowsCount >= list.columnsCount ? parent.height : list.rowsCount * parent.height/cell
            width: list.rowsCount <= list.columnsCount ? parent.width : list.columnsCount * parent.width/cell
            interactive: false
            cellHeight: parent.height/cell
            cellWidth: parent.height/cell
            verticalLayoutDirection: GridView.BottomToTop
            //        currentIndex: list.thisIndex
            delegate: delegate
            model: Engine {
                id: list
            }

            moveDisplaced: Transition {
                NumberAnimation {properties: "x,y"; duration: 300}
            }
            move: Transition {
                NumberAnimation { easing.type: Easing.OutSine; properties: "x,y"; duration: 300}
            }
            add: Transition {
                NumberAnimation {property: "opacity"; to: 1; duration: 200}
            }
            remove: Transition {
                ParallelAnimation {
                    NumberAnimation {property: "opacity"; to: 0; duration: 200}
                    NumberAnimation {properties: "y"; duration: 300}
                }
            }
        }
    }

    Rectangle {
        id: statusBar
        width: parent.width
        height: board.cellHeight
        color: "#133843"
        border.color: "#ffffff"
        border.width: 1

        Text {
            verticalAlignment: Text.AlignVCenter
            font.pointSize: 20
            color: "white"
            text: 'Moves: ' + list.movesCount + ' / ' + list.maxMoves + '    Score: ' + list.scoreCount + ' / ' + list.minScore
            anchors.verticalCenter: parent.verticalCenter
            horizontalAlignment: Text.AlignHCenter
        }
    }
    MessageDialog {
        property bool isVictory: false
        property bool isVisible: false

        id: messageDialog
        title: isVictory ? "Good!" : ".."
        text: isVictory ? "My congratulations!" : "..something went wrong.."
        visible: isVisible
        onAccepted: {
            list.resetBoard()
            isVictory = false
            isVisible = false
            list.startCount = false
        }
    }
}