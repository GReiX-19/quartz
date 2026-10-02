import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Quartz.Backend

ApplicationWindow {
    visible: true
    width: 900
    height: 600
    title: "Quartz"

    Player {
        id: player
    }

    PlaylistModel {
        id: playlist
        Component.onCompleted: scanMusicFolder()
    }

    function fmt(ms){
        const s = Math.floor(ms / 1000)
        const m = Math.floor(s / 60)
        return m + ":" + String(s % 60).padStart(2, "0")
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 24
            spacing: 16

            Rectangle {
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: 260
                Layout.preferredHeight: 260
                radius: 12
                color: "#33000000"
            }

            Slider {
                Layout.fillWidth: true
                from: 0
                to: player.duration
                value: player.position
                onMoved: player.seek(value)
            }

            Label {
                Layout.alignment: Qt.AlignHCenter
                text: fmt(player.position) + " / " + fmt(player.duration)
            }

            Button {
                Layout.alignment: Qt.AlignHCenter
                text: player.playing ? "Pause" : "Play"
                onClicked: player.toggle()
            }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: 12

                Button {
                    id: playlistButton
                    text: "List"
                    checkable: true
                    checked: true
                }
                Button {
                    text: "Search"
                    onClicked: {
                        playlistButton.checked = true
                        playlistPanel.focusSearch()
                    }
                }
                Label {
                    text: "Vol"
                }
                Slider {
                    from: 0
                    to: 1
                    value: player.volume
                    onMoved: player.volume = value
                }
            }

            Item {
                Layout.fillHeight: true
            }
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: 1
            visible: playlistButton.checked
            color: "#33000000"
        }

        PlaylistPanel {
            id: playlistPanel
            Layout.fillHeight: true
            Layout.preferredWidth: 320
            visible: playlistButton.checked
            model: playlist
            currentSource: player.source
            onTrackActivated: (url) => {
                player.source = url
                player.play()
            }
        }
    }
}