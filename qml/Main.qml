import QtQuick
import QtQuick.Controls
import Quartz.Backend

ApplicationWindow {
    visible: true
    width: 600
    height: 600
    title: "Quartz"

    Player {
        id: player
    }

    Button {
        anchors.centerIn: parent
        text: player.playing ? "Pause" : "Play"
        onClicked: player.toggle()
    }
}