#include "PlaylistModel.h"

#include <QDirIterator>
#include <QFileInfo>
#include <QStandardPaths>
#include <algorithm>
#include <numeric>
#include <random>

PlaylistModel::PlaylistModel(QObject* _parent)
    : QAbstractListModel(_parent) {

}

int PlaylistModel::count() const {
    return static_cast<int>(m_tracks.size());
}
int PlaylistModel::currentIndex() const {
    return m_currentIndex;
}
PlaylistModel::RepeatMode PlaylistModel::repeatMode() const {
    return m_repeatMode;
}
bool PlaylistModel::shuffle() const {
    return m_shuffle;
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

void PlaylistModel::setRepeatMode(RepeatMode _mode) {
    if (m_repeatMode == _mode)
        return;

    m_repeatMode = _mode;
    emit repeatModeChanged();
}

void PlaylistModel::setShuffle(bool _enabled) {
    if (m_shuffle == _enabled)
        return;

    m_shuffle = _enabled;
    if (m_shuffle)
        rebuildOrder(m_currentIndex);

    emit shuffleChanged();
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
    m_order.clear();
    m_orderPos = -1;
    if (m_shuffle)
        rebuildOrder(-1);
    endResetModel();

    emit countChanged();
    emit currentIndexChanged();
}

void PlaylistModel::playAt(int _row) {
    if (_row < 0 or _row >= count())
        return;

    if (m_shuffle)
        rebuildOrder(_row);

    setCurrent(_row);
}

bool PlaylistModel::next() {
    if (count() == 0)
        return false;

    int target;
    if (!m_shuffle) {
        target = m_currentIndex + 1;
        if (target >= count()) {
            if (m_repeatMode != RepeatPlaylist)
                return false;
            target = 0;
        }
    }
    else {
        int pos = m_orderPos + 1;
        if (pos >= count()) {
            if (m_repeatMode != RepeatPlaylist)
                return false;
            rebuildOrder(-1);

            if (count() > 1 and m_order.first() == m_currentIndex)
                std::swap(m_order.first(), m_order.last());
            pos = 0;
        }
        m_orderPos = pos;
        target = m_order[pos];
    }

    setCurrent(target);
    return true;
}

bool PlaylistModel::previous() {
    if (count() == 0)
        return false;

    int target;
    if (!m_shuffle) {
        target = m_currentIndex - 1;
        if (target < 0) {
            if (m_repeatMode != RepeatPlaylist)
                return false;
            target = count() - 1;
        }
    }
    else {
        int pos = m_orderPos - 1;
        if (pos < 0) {
            if (m_repeatMode != RepeatPlaylist)
                return false;
            pos = count() - 1;
        }
        m_orderPos = pos;
        target = m_order[pos];
    }

    setCurrent(target);
    return true;
}

void PlaylistModel::cycleRepeatMode() {
    setRepeatMode(static_cast<RepeatMode>((m_repeatMode + 1) % 3));
}

void PlaylistModel::trackFinished() {
    if (count() == 0 or m_currentIndex < 0)
        return;
    if (m_repeatMode == RepeatTrack)
        setCurrent(m_currentIndex);
    else
        next();
}

QString PlaylistModel::currentTitle() const {
    if (m_currentIndex >= count() or m_currentIndex < 0)
        return {};

    return m_tracks[m_currentIndex].title;
}

void PlaylistModel::setCurrent(int _row) {
    if (m_currentIndex != _row) {
        m_currentIndex = _row;
        emit currentIndexChanged();
    }

    emit playRequested(m_tracks[_row].url);
}

void PlaylistModel::rebuildOrder(int _first) {
    m_order.resize(count());
    std::ranges::iota(m_order, 0);

    std::mt19937 rng{ std::random_device{} () };

    if (_first >= 0 and _first < count()) {
        std::swap(m_order[0], m_order[_first]);
        std::ranges::shuffle(m_order, rng);
        m_orderPos = 0;
    }
    else {
        std::ranges::shuffle(m_order, rng);
        m_orderPos = -1;
    }
}