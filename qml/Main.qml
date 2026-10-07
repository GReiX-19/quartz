import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Quartz.Backend

ApplicationWindow {
    id: root
    visible: true
    title: "Quartz"

    readonly property int playlistWidth: 319
    readonly property int dividerWidth: 1

    property bool playlistOpen: false
    property bool volumeOpen: false

    width: 900
    height: 600
    minimumWidth: 480
    minimumHeight: 520

    onVolumeOpenChanged: {
        if (volumeOpen)
            volumeSlider.forceActiveFocus()
    }

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

    function showSearch() {
        playlistOpen = true
        playlistPanel.focusSearch()
    }

    Shortcut { sequence: "Space"; onActivated: player.toggle() }
    Shortcut { sequence: "N"; onActivated: playlist.next() }
    Shortcut { sequence: "P"; onActivated: playlist.previous() }
    Shortcut { sequence: "R"; onActivated: playlist.cycleRepeatMode() }
    Shortcut { sequence: "H"; onActivated: playlist.shuffle = !playlist.shuffle }
    Shortcut { sequence: "L"; onActivated: root.playlistOpen = !root.playlistOpen }
    Shortcut { sequence: "S"; onActivated: root.showSearch() }
    Shortcut { sequence: "V"; onActivated: root.volumeOpen = !root.volumeOpen }
    Shortcut { sequence: "D"; onActivated: durationSlider.focus = !durationSlider.focus }

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
                        text: "List"
                        checkable: true
                        checked: root.playlistOpen
                        onToggled: root.playlistOpen = checked
                    }
                    Button {
                        implicitWidth: 40
                        implicitHeight: 40
                        text: "Search"
                        onClicked: root.showSearch()
                    }
                    Button {
                        id: volButton
                        implicitWidth: 40
                        implicitHeight: 40
                        text: "Volume"
                        checked: root.volumeOpen
                        onClicked: root.volumeOpen = checked
                    }
                    Slider {
                        id: volumeSlider
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredHeight: 130
                        visible: root.volumeOpen
                        orientation: Qt.Vertical
                        from: 0
                        to: 1
                        stepSize: 0.05
                        value: player.volume
                        onMoved: player.volume = value
                        Keys.onEscapePressed: root.volumeOpen = false
                    }
                }

                ColumnLayout {
                    anchors.centerIn: parent
                    spacing: 10

                    Label {
                        id: trackTitle
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredWidth: 300
                        Layout.maximumWidth: 300
                        readonly property bool hasTrack: playlist.currentIndex >= 0

                        text: hasTrack ? playlist.currentTitle : "Nothing plays."
                        opacity: hasTrack ? 1.0 : 0.6
                        font.pixelSize: 15
                        horizontalAlignment: Text.AlignHCenter
                        elide: Text.ElideRight
                        maximumLineCount: 1

                        HoverHandler { id: titleHover }
                        ToolTip.visible: titleHover.hovered && trackTitle.truncated
                        ToolTip.text: trackTitle.text
                        ToolTip.delay: 500
                    }

                    Rectangle {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredWidth: 260
                        Layout.preferredHeight: 260
                        radius: 12
                        color: "#33000000"
                    }

                    Slider {
                        id: durationSlider
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredWidth: 300
                        from: 0
                        to: player.duration
                        stepSize: 5000
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
            Layout.preferredWidth: root.dividerWidth
            visible: root.playlistOpen
            color: "#33000000"
        }

        PlaylistPanel {
            id: playlistPanel
            Layout.fillHeight: true
            Layout.preferredWidth: root.playlistWidth
            visible: root.playlistOpen
            model: filteredPlaylist
            currentIndex: playlist.currentIndex
            onTrackActivated: (row) => playlist.playAt(row)
        }
    }
}