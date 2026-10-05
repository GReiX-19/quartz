#pragma once

#include <QObject>
#include <QAudioOutput>
#include <QMediaPlayer>
#include <QUrl>
#include <QtQml/qqmlregistration.h>

class Player : public QObject {
    Q_OBJECT;
    QML_ELEMENT;

    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged);
    Q_PROPERTY(bool playing READ playing NOTIFY playingChanged);
    Q_PROPERTY(qint64 position READ position NOTIFY positionChanged);
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged);
    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged);
    Q_PROPERTY(PlaybackState playbackState READ playbackState NOTIFY playbackStateChanged);
    enum PlaybackState {
        Stopped,
        Paused,
        Playing,
    };
    Q_ENUM(PlaybackState);

public:
    explicit Player(QObject* _parent = nullptr);

    QUrl source() const;
    void setSource(const QUrl& _url);

    bool playing() const;
    qint64 position() const;
    qint64 duration() const;
    PlaybackState playbackState() const;

    float volume() const;
    void setVolume(float _v);

    Q_INVOKABLE void play();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void toggle();
    Q_INVOKABLE void seek(qint64 _ms);
    Q_INVOKABLE void stop();

signals:
    void sourceChanged();
    void playingChanged();
    void positionChanged();
    void durationChanged();
    void volumeChanged();
    void finished();
    void playbackStateChanged();

private:
    QAudioOutput m_output;
    QMediaPlayer m_player;
};