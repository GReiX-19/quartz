#include "MprisService.hpp"

#include <QDBusConnection>
#include <QDBusError>
#include <QDBusMessage>
#include <QDebug>

namespace {
    constexpr auto kObjectPath = "/org/mpris/MediaPlayer2";
    constexpr auto kServiceName = "org.mpris.MediaPlayer2.quartz";
    constexpr auto kPlayerInterface = "org.mpris.MediaPlayer2.Player";
    constexpr auto kNoTrack = "org/mpris/MediaPlayer2/TrackList/NoTrack";
}

MprisService::MprisService(QObject* _parent) : QObject(_parent) {
    new MprisRootAdaptor(this);
    m_playerAdaptor = new MprisPlayerAdaptor(this);

    auto bus = QDBusConnection::sessionBus();
    if (!bus.registerObject(QString::fromLatin1(kObjectPath), this, QDBusConnection::ExportAdaptors)) {
        qWarning() << "MPRIS: could not register object:" << bus.lastError().message();
        return;
    }

    if (!bus.registerService(QString::fromLatin1(kServiceName))) {
        qWarning() << "MPRIS: cound not register service:" << bus.lastError().message();
    }
}

void MprisService::setPlayer(Player* _player) {
    if (m_player == _player)
        return;
    if (m_player)
        m_player->disconnect(this);

    m_player = _player;

    if (m_player) {
        connect(m_player, &Player::playbackStateChanged, this, [this] {
            notifyPlayerProperties({ { QStringLiteral("PlaybackStatus"), playbackStatus() } });
            });
        connect(m_player, &Player::durationChanged, this, [this] {
            notifyPlayerProperties({ { QStringLiteral("Metadata"), metadata() } });
            });
        connect(m_player, &Player::volumeChanged, this, [this] {
            notifyPlayerProperties({ { QStringLiteral("Volume"), volume() } });
            });
        connect(m_player, &Player::seeked, this, [this](qint64 _ms) {
            emit m_playerAdaptor->Seeked(_ms * 1000);
            });
    }

    emit playerChanged();
}

void MprisService::setPlaylist(PlaylistModel* _playlist) {
    if (m_playlist == _playlist)
        return;
    if (m_playlist)
        m_playlist->disconnect(this);
    m_playlist = _playlist;

    if (m_playlist) {
        connect(m_playlist, &PlaylistModel::currentIndexChanged, this, [this] {
            notifyPlayerProperties({ { QStringLiteral("Metadata"), metadata() } });
            });
        connect(m_playlist, &PlaylistModel::countChanged, this, [this] {
            const bool has = hasTracks();
            notifyPlayerProperties({ { QStringLiteral("CanGoNext"), has },
                                    { QStringLiteral("CanGoPrevious"), has },
                                    { QStringLiteral("CanPlay"), has } });
            });
        connect(m_playlist, &PlaylistModel::shuffleChanged, this, [this] {
            notifyPlayerProperties({ { QStringLiteral("Shuffle"), shuffle() } });
            });
        connect(m_playlist, &PlaylistModel::repeatModeChanged, this, [this] {
            notifyPlayerProperties({ { QStringLiteral("LoopStatus"), loopStatus() } });
            });
    }

    emit playlistChanged();
}

bool MprisService::shuffle() const {
    return m_playlist and m_playlist->shuffle();
}
void MprisService::setShuffle(bool _enabled) {
    if (m_playlist)
        m_playlist->setShuffle(_enabled);
}
QString MprisService::loopStatus() const {
    if (!m_playlist)
        return QStringLiteral("None");

    switch (m_playlist->repeatMode()) {
    case PlaylistModel::RepeatPlaylist: return QStringLiteral("Playlist");
    case PlaylistModel::RepeatTrack: return QStringLiteral("Track");
    default: return QStringLiteral("None");
    }
}
void MprisService::setLoopStatus(const QString& _status) {
    if (!m_playlist)
        return;

    if (_status == QLatin1String("None"))
        m_playlist->setRepeatMode(PlaylistModel::RepeatOff);
    else if (_status == QLatin1String("Playlist"))
        m_playlist->setRepeatMode(PlaylistModel::RepeatPlaylist);
    else if (_status == QLatin1String("Track"))
        m_playlist->setRepeatMode(PlaylistModel::RepeatTrack);
}

QString MprisService::playbackStatus() const {
    if (!m_player)
        return QStringLiteral("Stopped");
    switch (m_player->playbackState()) {
    case Player::Playing:
        return QStringLiteral("Playing");
    case Player::Paused:
        return QStringLiteral("Paused");
    default:
        return QStringLiteral("Stopped");
    }

}
QVariantMap MprisService::metadata() const {
    QVariantMap map;
    map.insert(QStringLiteral("mpris:trackid"),
        QVariant::fromValue(QDBusObjectPath(currentTrackPath())));

    if (!m_playlist or m_playlist->currentIndex() < 0)
        return map;

    map.insert(QStringLiteral("xesam:title"), m_playlist->currentTitle());
    if (m_player) {
        map.insert(QStringLiteral("mpris:length"), qlonglong(m_player->duration()) * 1000);
        map.insert(QStringLiteral("xesam:url"), m_player->source().toString());
    }

    return map;
}
qlonglong MprisService::positionUs() const {
    return m_player ? qlonglong(m_player->position()) * 1000 : 0;
}
double MprisService::volume() const {
    return m_player ? double(m_player->volume()) : 0.0;
}
void MprisService::setVolume(double _v) {
    if (m_player)
        m_player->setVolume(static_cast<float>(_v));
}
bool MprisService::hasTracks() const {
    return m_playlist and m_playlist->count() > 0;
}

void MprisService::play() {
    if (!m_player)
        return;
    if (m_player->playbackState() == Player::Stopped and m_playlist
        and m_playlist->currentIndex() < 0) {
        m_playlist->next();
        return;
    }

    m_player->play();
}
void MprisService::pause() {
    if (m_player)
        m_player->pause();
}
void MprisService::playPause() {
    if (m_player and m_player->playing())
        pause();
    else
        play();
}
void MprisService::stop() {
    if (m_player)
        m_player->stop();
}
void MprisService::next() {
    if (m_playlist)
        m_playlist->next();
}
void MprisService::previous() {
    if (m_playlist)
        m_playlist->previous();
}
void MprisService::seekBy(qlonglong offsetUs) {
    if (!m_player)
        return;

    const qint64 target = qMax<qint64>(0, m_player->position() + offsetUs / 1000);
    m_player->seek(target);

}
void MprisService::setPositionUs(const QString& trackPath, qlonglong us) {
    if (!m_player or trackPath != currentTrackPath() or us < 0)
        return;

    m_player->seek(us / 1000);
}

QString MprisService::currentTrackPath() const {
    if (!m_playlist or m_playlist->currentIndex() < 0)
        return QString::fromLatin1(kNoTrack);

    return QStringLiteral("/org/mpris/MediaPlayer2/Track/%1").arg(m_playlist->currentIndex());
}
void MprisService::notifyPlayerProperties(const QVariantMap& changed) {
    QDBusMessage msg = QDBusMessage::createSignal(
        QString::fromLatin1(kObjectPath),
        QStringLiteral("org.freedesktop.DBus.Properties"),
        QStringLiteral("PropertiesChanged"));

    msg << QString::fromLatin1(kPlayerInterface) << changed << QStringList();

    QDBusConnection::sessionBus().send(msg);
}
