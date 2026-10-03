#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QUrl>
#include <QtQml/qqmlregistration.h>

class PlaylistModel : public QAbstractListModel {
    Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(int count READ count NOTIFY countChanged)
        Q_PROPERTY(int currentIndex READ currentIndex NOTIFY currentIndexChanged)

public:
    enum Roles {
        TitleRole = Qt::UserRole + 1,
        TrackUrlRole,
    };

    explicit PlaylistModel(QObject* _parent = nullptr);

    int count() const;
    int currentIndex() const;

    int rowCount(const QModelIndex& _parent = {}) const override;
    QVariant data(const QModelIndex& _index, int _role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void scanMusicFolder();
    Q_INVOKABLE void playAt(int _row);
    Q_INVOKABLE bool next();
    Q_INVOKABLE bool previous();

signals:
    void countChanged();
    void currentIndexChanged();
    void playRequested(const QUrl& _trackUrl);

private:
    struct Track {
        QString title;
        QUrl url;
    };

    QList<Track> m_tracks;
    int m_currentIndex = -1;
};