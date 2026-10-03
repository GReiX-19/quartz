#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QUrl>
#include <QtQml/qqmlregistration.h>

class PlaylistModel : public QAbstractListModel {
    Q_OBJECT;
    QML_ELEMENT;
    Q_PROPERTY(int count READ count NOTIFY countChanged);
    Q_PROPERTY(int currentIndex READ currentIndex NOTIFY currentIndexChanged);
    Q_PROPERTY(RepeatMode repeatMode READ repeatMode WRITE setRepeatMode NOTIFY repeatModeChanged);

public:
    enum Roles {
        TitleRole = Qt::UserRole + 1,
        TrackUrlRole,
    };

    enum RepeatMode {
        RepeatOff,
        RepeatPlaylist,
        RepeatTrack,
    };
    Q_ENUM(RepeatMode);

    explicit PlaylistModel(QObject* _parent = nullptr);

    int count() const;
    int currentIndex() const;
    RepeatMode repeatMode() const;

    int rowCount(const QModelIndex& _parent = {}) const override;
    QVariant data(const QModelIndex& _index, int _role) const override;
    QHash<int, QByteArray> roleNames() const override;
    void setRepeatMode(RepeatMode _mode);

    Q_INVOKABLE void scanMusicFolder();
    Q_INVOKABLE void playAt(int _row);
    Q_INVOKABLE bool next();
    Q_INVOKABLE bool previous();
    Q_INVOKABLE void cycleRepeatMode();
    Q_INVOKABLE void trackFinished();

signals:
    void countChanged();
    void currentIndexChanged();
    void playRequested(const QUrl& _trackUrl);
    void repeatModeChanged();

private:
    struct Track {
        QString title;
        QUrl url;
    };

    QList<Track> m_tracks;
    int m_currentIndex = -1;
    RepeatMode m_repeatMode = RepeatOff;
};