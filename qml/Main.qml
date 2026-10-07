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
        onFinished: playlist.trackFinished()
    }

    PlaylistModel {
        id: playlist
        Component.onCompleted: scanMusicFolder()
        onPlayRequested: (trackUrl) => {
            player.source = trackUrl
            player.play()
        }
    }

    MprisService {
        engine: player
        tracks: playlist
    }

    TrackFilterModel {
        id: filteredPlaylist
        sourceModel: playlist
        filterText: playlistPanel.searchText
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

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                ColumnLayout {
                    id: rightControls
                    anchors.top: parent.top
                    anchors.right: parent.right
                    spacing: 10

                    Button {
                        implicitWidth: 40
                        implicitHeight: 40
                        id: playlistButton
                        text: "List"
                        checkable: true
                        checked: false
                    }
                    Button {
                        implicitWidth: 40
                        implicitHeight: 40
                        text: "Search"
                        onClicked: {
                            playlistButton.checked = true
                            playlistPanel.focusSearch()
                        }
                    }
                    Button {
                        implicitWidth: 40
                        implicitHeight: 40
                        id: volButton
                        text: "Volume"
                        checkable: true
                        checked: false
                    }
                    Slider {
                        Layout.preferredHeight: 130
                        visible: volButton.checked
                        from: 0
                        to: 1
                        value: player.volume
                        onMoved: player.volume = value
                        orientation: Qt.Vertical
                    }
                }

                ColumnLayout {
                    anchors.centerIn: parent
                    spacing: 10

                    Label {
                        id: trackTitle
                        Layout.alignment: Qt.AlignHCenter
                        readonly property bool hasTrack: playlist.currentIndex >= 0
                        text: hasTrack ? playlist.currentTitle : "Nothing plays."
                        opacity: hasTrack ? 1.0 : 0.6
                    }

                    Rectangle {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredWidth: 260
                        Layout.preferredHeight: 260
                        radius: 12
                        color: "#33000000"
                    }

                    Slider {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredWidth: 300
                        from: 0
                        to: player.duration
                        value: player.position
                        onMoved: player.seek(value)
                    }

                    Label {
                        Layout.alignment: Qt.AlignHCenter
                        text: fmt(player.position) + " / " + fmt(player.duration)
                    }

                    RowLayout {
                        Layout.alignment: Qt.AlignHCenter
                        spacing: 8

                        Button {
                            implicitWidth: 40
                            implicitHeight: 40
                            text: "Shuffle"
                            checkable: true
                            checked: playlist.shuffle
                            onToggled: playlist.shuffle = checked
                        }
                        Button {
                            implicitWidth: 40
                            implicitHeight: 40
                            text: "Prev"
                            onClicked: playlist.previous()
                        }
                        Button {
                            implicitWidth: 40
                            implicitHeight: 40
                            text: player.playing ? "Pause" : "Play"
                            onClicked: player.toggle()
                        }
                        Button {
                            implicitWidth: 40
                            implicitHeight: 40
                            text: "Next"
                            onClicked: playlist.next()
                        }
                        Button {
                            implicitWidth: 40
                            implicitHeight: 40
                            text: playlist.repeatMode === PlaylistModel.RepeatOff ? "Repeat: Off"
                                : playlist.repeatMode === PlaylistModel.RepeatPlaylist ? "Repeat: All"
                                : "Repeat: One"
                            onClicked: playlist.cycleRepeatMode()
                        }
                    }
                }
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
            model: filteredPlaylist
            currentIndex: playlist.currentIndex
            onTrackActivated: (row) => playlist.playAt(row)
        }
    }
}