import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property alias model: list.model
    property alias searchText: searchField.text
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
            Keys.onEscapePressed: {
                if (text.length > 0) 
                    text = ""
                else
                    focus = false
            }
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            visible: list.count === 0 && searchField.text.length > 0
            text: "No tracks found"
            opacity: 0.6
        }

        ListView {
            id: list
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            ScrollBar.vertical: ScrollBar {}

            delegate: ItemDelegate {
                required property int sourceRow
                required property string title

                width: ListView.view.width
                text: title
                highlighted: sourceRow === root.currentIndex
                onClicked: root.trackActivated(sourceRow)
            }
        }
    }
}