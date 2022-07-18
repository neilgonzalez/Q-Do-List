import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import ToDo 1.0

//grid having one column
ColumnLayout{

    Frame{
        Layout.fillHeight: true

        ListView{

            implicitWidth: 250
            implicitHeight: 250

            clip: true
            anchors.fill: parent
            model: ToDoModel {
                list: toDoList
            }

            delegate: RowLayout {



                CheckBox {

                    checked: model.done
                    onClicked: model.done = checked
                }
                TextField {
                    text: model.description
                    onEditingFinished: model.description = text
                    Layout.fillWidth: true
                }
            }

        }
    }
    //grid with one row
    RowLayout{
        Button{
            text: qsTr("Add new item")
            onClicked: toDoList.appendItem()
            Layout.fillWidth: true
        }
        Button{
            text: qsTr("Remove completed")
            onClicked: toDoList.removeCompletedItems()
            Layout.fillWidth: true
        }
    }
}
