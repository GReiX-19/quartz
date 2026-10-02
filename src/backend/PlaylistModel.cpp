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

    // std::ranges::sort(found, [](const Track& a, Track& b) {
    //     return a.title.localeAwareCompare(b.title) < 0;
    //     });

    beginResetModel();
    m_tracks = std::move(found);
    endResetModel();

    emit countChanged();
}