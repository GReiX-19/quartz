#pragma once

#include "Player.hpp"
#include "PlaylistModel.hpp"

#include <QCoreApplication>
#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>
#include <QObject>
#include <QPointer>
#include <QStringList>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>

class MprisPlayerAdaptor;

class MprisService : public QObject {
    Q_OBJECT;
    QML_ELEMENT;
    Q_PROPERTY(Player* engine READ player WRITE setPlayer NOTIFY playerChanged);
    Q_PROPERTY(PlaylistModel* tracks READ playlist WRITE setPlaylist NOTIFY playlistChanged)

public: //methods
    explicit MprisService(QObject* _parent = nullptr);

    Player* player() const { return m_player; }
    void setPlayer(Player* _player);
    PlaylistModel* playlist() const { return m_playlist; }
    void setPlaylist(PlaylistModel* _playlist);

    QString playbackStatus() const;
    QVariantMap metadata() const;
    qlonglong positionUs() const;
    double volume() const;
    void setVolume(double _v);
    bool hasTracks() const;

    void play();
    void pause();
    void playPause();
    void stop();
    void next();
    void previous();
    void seekBy(qlonglong _offsetUs);
    void setPositionUs(const QString& _trackPath, qlonglong _us);

signals:
    void playerChanged();
    void playlistChanged();

private: //methods
    QString currentTrackPath() const;
    void notifyPlayerProperties(const QVariantMap& _changed);

private: //members
    QPointer<Player> m_player;
    QPointer<PlaylistModel> m_playlist;
    MprisPlayerAdaptor* m_playerAdaptor = nullptr;
};

class MprisRootAdaptor : public QDBusAbstractAdaptor {
    Q_OBJECT;
    Q_CLASSINFO("D-Bus interface", "org.mpris.MediaPlayer2");
    Q_PROPERTY(bool CanQuit READ canQuit);
    Q_PROPERTY(bool CanRaise READ canRaise);
    Q_PROPERTY(bool HasTrackList READ hasTrackList);
    Q_PROPERTY(QString Identity READ identity);
    Q_PROPERTY(QString DesktopEntry READ desktopEntry);
    Q_PROPERTY(QStringList SupportedUriSchemes READ supportedUriSchemes);
    Q_PROPERTY(QStringList SupportedMimeTypes READ supportedMimeTypes);

public: //methods
    explicit MprisRootAdaptor(QObject* _parent)
        : QDBusAbstractAdaptor(_parent) {
    }

    bool canQuit() const { return true; }
    bool canRaise() const { return false; }
    bool hasTrackList() const { return false; }
    QString identity() const { return QStringLiteral("Quartz"); }
    QString desktopEntry() const { return QStringLiteral("quartz"); }
    QStringList supportedUriSchemes() const { return { QStringLiteral("file") }; }
    QStringList supportedMimeTypes() const {
        return { QStringLiteral("audio/mpeg"), QStringLiteral("audio/flac"),
                QStringLiteral("audio/ogg"), QStringLiteral("audio/x-wav"),
                QStringLiteral("audio/mp4"), QStringLiteral("audio/mp3")
        };
    }

public slots:
    void Raise() {}
    void Quit() { QCoreApplication::quit(); }
};

class MprisPlayerAdaptor : public QDBusAbstractAdaptor {
    Q_OBJECT;
    Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2.Player");
    Q_PROPERTY(QString PlaybackStatus READ playbackStatus);
    Q_PROPERTY(QVariantMap Metadata READ metadata);
    Q_PROPERTY(qlonglong Position READ position);
    Q_PROPERTY(double Volume READ volume WRITE setVolume);
    Q_PROPERTY(double Rate READ rate);
    Q_PROPERTY(double MinimumRate READ rate);
    Q_PROPERTY(double MaximumRate READ rate);
    Q_PROPERTY(bool CanGoNext READ canGoNext);
    Q_PROPERTY(bool CanGoPrevious READ canGoPrevious);
    Q_PROPERTY(bool CanPlay READ canPlay);
    Q_PROPERTY(bool CanPause READ canPause);
    Q_PROPERTY(bool CanSeek READ canSeek);
    Q_PROPERTY(bool CanControl READ canControl);

public: //methods
    explicit MprisPlayerAdaptor(MprisService* _service)
        : QDBusAbstractAdaptor(_service), m_service(_service) {
    }

    QString playbackStatus() const { return m_service->playbackStatus(); }
    QVariantMap metadata() const { return m_service->metadata(); }
    qlonglong position() const { return m_service->positionUs(); }
    double volume() const { return m_service->volume(); }
    void setVolume(double v) { m_service->setVolume(v); }
    double rate() const { return 1.0; }
    bool canGoNext() const { return m_service->hasTracks(); }
    bool canGoPrevious() const { return m_service->hasTracks(); }
    bool canPlay() const { return m_service->hasTracks(); }
    bool canPause() const { return true; }
    bool canSeek() const { return m_service->player() != nullptr; }
    bool canControl() const { return true; }

public slots:
    void Next() { m_service->next(); }
    void Previous() { m_service->previous(); }
    void Pause() { m_service->pause(); }
    void PlayPause() { m_service->playPause(); }
    void Stop() { m_service->stop(); }
    void Play() { m_service->play(); }
    void Seek(qlonglong _offset) { m_service->seekBy(_offset); }
    void SetPosition(const QDBusObjectPath& _trackId, qlonglong _position) {
        m_service->setPositionUs(_trackId.path(), _position);
    }
    void OpenUri(const QString&) {}

signals:
    void Seeked(qlonglong _Position);

private: //members
    MprisService* m_service;
};