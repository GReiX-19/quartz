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
    Q_PROPERTY(bool shuffle READ shuffle WRITE setShuffle NOTIFY shuffleChanged);
    Q_PROPERTY(QString currentTitle READ currentTitle NOTIFY currentIndexChanged)

public:
    enum Roles {
        TitleRole = Qt::UserRole + 1,
        TrackUrlRole,
        SourceRowRole,
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
    bool shuffle() const;

    int rowCount(const QModelIndex& _parent = {}) const override;
    QVariant data(const QModelIndex& _index, int _role) const override;
    QHash<int, QByteArray> roleNames() const override;
    void setRepeatMode(RepeatMode _mode);
    void setShuffle(bool _enabled);

    Q_INVOKABLE void scanMusicFolder();
    Q_INVOKABLE void playAt(int _row);
    Q_INVOKABLE bool next();
    Q_INVOKABLE bool previous();
    Q_INVOKABLE void cycleRepeatMode();
    Q_INVOKABLE void trackFinished();
    Q_INVOKABLE QString currentTitle() const;

signals:
    void countChanged();
    void currentIndexChanged();
    void playRequested(const QUrl& _trackUrl);
    void repeatModeChanged();
    void shuffleChanged();

private:
    void setCurrent(int _row);
    void rebuildOrder(int _first);

private:
    struct Track {
        QString title;
        QUrl url;
    };

    QList<Track> m_tracks;
    int m_currentIndex = -1;
    RepeatMode m_repeatMode = RepeatOff;
    bool m_shuffle = false;
    QList<int> m_order;
    int m_orderPos = -1;
};