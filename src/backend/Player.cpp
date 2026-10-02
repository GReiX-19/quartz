#include "Player.h"

#include <QDebug>

Player::Player(QObject* _parent)
    : QObject(_parent) {

    m_player.setAudioOutput(&m_output);
    m_output.setVolume(0.6f);

    connect(&m_player, &QMediaPlayer::sourceChanged, this, &Player::sourceChanged);
    connect(&m_player, &QMediaPlayer::playbackStateChanged, this, &Player::playingChanged);
    connect(&m_player, &QMediaPlayer::positionChanged, this, &Player::positionChanged);
    connect(&m_player, &QMediaPlayer::durationChanged, this, &Player::durationChanged);
    connect(&m_output, &QAudioOutput::volumeChanged, this, &Player::volumeChanged);

    connect(&m_player, &QMediaPlayer::errorOccurred, this,
        [](QMediaPlayer::Error error, const QString& message) {
            qWarning() << "Player error: " << error << message;
        });
}

QUrl Player::source() const {
    return m_player.source();
}
void Player::setSource(const QUrl& _url) {
    m_player.setSource(_url);
}

bool Player::playing() const {
    return m_player.playbackState() == QMediaPlayer::PlayingState;
}
qint64 Player::position() const {
    return m_player.position();
}
qint64 Player::duration() const {
    return m_player.duration();
}

float Player::volume() const {
    return m_output.volume();
}
void Player::setVolume(float _v) {
    m_output.setVolume(qBound(0.0f, _v, 1.0f));
}

void Player::play() {
    m_player.play();
}
void Player::pause() {
    m_player.pause();
}
void Player::toggle() {
    if (playing())
        pause();
    else
        play();
}
void Player::seek(qint64 _ms) {
    m_player.setPosition(_ms);
}