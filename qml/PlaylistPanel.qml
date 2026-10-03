import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property alias model: list.model
    property int currentIndex: -1
    signal trackActivated(int row)

    function focusSearch() {
        searchField.forceActiveFocus()
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        TextField {
            id: searchField
            Layout.fillWidth: true
            placeholderText: "Search"
        }

        ListView {
            id: list
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            ScrollBar.vertical: ScrollBar {}

            delegate: ItemDelegate {
                required property int index
                required property string title

                width: ListView.view.width
                text: title
                highlighted: index === root.currentIndex
                onClicked: root.trackActivated(index)
            }
        }
    }
}