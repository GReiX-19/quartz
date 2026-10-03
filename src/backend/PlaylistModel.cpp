#include "PlaylistModel.h"

#include <QDirIterator>
#include <QFileInfo>
#include <QStandardPaths>
#include <algorithm>

PlaylistModel::PlaylistModel(QObject* _parent)
    : QAbstractListModel(_parent) {

}

int PlaylistModel::count() const {
    return static_cast<int>(m_tracks.size());
}
int PlaylistModel::currentIndex() const {
    return m_currentIndex;
}

int PlaylistModel::rowCount(const QModelIndex& _parent) const {
    return _parent.isValid() ? 0 : static_cast<int>(m_tracks.size());
}

QVariant PlaylistModel::data(const QModelIndex& _index, int _role) const {
    if (!_index.isValid() or _index.row() < 0 or _index.row() >= count())
        return {};

    const Track& track = m_tracks[_index.row()];

    switch (_role) {
    case TitleRole:
        return track.title;
    case TrackUrlRole:
        return track.url;
    default:
        return {};
    }
}

QHash<int, QByteArray> PlaylistModel::roleNames() const {
    return {
        { TitleRole, "title" },
        { TrackUrlRole, "trackUrl" },
    };
}

void PlaylistModel::scanMusicFolder() {
    const QString musicDir = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);

    QList<Track> found;
    QDirIterator it(musicDir
        , { "*.mp3", "*.flac", "*.ogg", "*.opus", "*.wav", "*.m4a" }
        , QDir::Files
        , QDirIterator::Subdirectories);

    while (it.hasNext()) {
        const QString path = it.next();
        found.append({ QFileInfo(path).completeBaseName(), QUrl::fromLocalFile(path) });
    }

    std::ranges::reverse(found);

    beginResetModel();
    m_tracks = std::move(found);
    m_currentIndex = -1;
    endResetModel();

    emit countChanged();
    emit currentIndexChanged();
}

void PlaylistModel::playAt(int _row) {
    if (_row < 0 or _row >= count())
        return;

    if (m_currentIndex != _row) {
        m_currentIndex = _row;
        emit currentIndexChanged();
    }

    emit playRequested(m_tracks[_row].url);
}

bool PlaylistModel::next() {
    if (m_currentIndex + 1 >= count())
        return false;

    playAt(m_currentIndex + 1);
    return true;
}

bool PlaylistModel::previous() {
    if (m_currentIndex <= 0)
        return false;

    playAt(m_currentIndex - 1);
    return true;
}