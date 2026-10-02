#include "Player.h"

Player::Player(QObject* _parent)
    : QObject(_parent) {
}

bool Player::playing() const {
    return m_playing;
}

void Player::toggle() {
    m_playing = !m_playing;
    emit playingChanged();
}