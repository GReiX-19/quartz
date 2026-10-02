import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property alias model: list.model
    property url currentSource
    signal trackActivated(url trackUrl)

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
                required property string title
                required property url trackUrl

                width: ListView.view.width
                text: title
                highlighted: root.currentSource.toString() === trackUrl.toString()
                onClicked: root.trackActivated(trackUrl)
            }
        }
    }
}