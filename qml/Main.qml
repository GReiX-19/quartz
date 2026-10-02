import QtQuick
import QtQuick.Controls
import Quartz.Backend

ApplicationWindow {
    visible: true
    width: 420
    height: 360
    title: "Quartz"

    Player {
        id: player
        source: "file:///home/GReiX19/Music/addiction.mp3"
    }

    function fmt(ms){
        const s = Math.floor(ms / 1000)
        const m = Math.floor(s / 60)
        return m + ":" + String(s % 60).padStart(2, "0")
    }

    Column{
        anchors.centerIn: parent
        width: parent.width - 60
        spacing: 16

        Slider {
            id: seekSlider
            width: parent.width
            from: 0
            to: player.duration
            value: player.position
            onMoved: player.seek(value)
        }

        Label {
            text: fmt(player.position) + " / " + fmt(player.duration)
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Button {
            text: player.playing ? "Pause" : "Play"
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: player.toggle()
        }

        Row {
            spacing: 10
            anchors.horizontalCenter: parent.horizontalCenter
            
            Label {
                text: "Vol"
                anchors.verticalCenter: parent.verticalCenter
            }
            Slider {
                from: 0
                to: 1
                value: player.volume
                onMoved: player.volume = value
            }
        }
    }
}