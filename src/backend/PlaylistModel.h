#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QUrl>
#include <QtQml/qqmlregistration.h>

class PlaylistModel : public QAbstractListModel {
    Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum Roles {
        TitleRole = Qt::UserRole + 1,
        TrackUrlRole,
    };

    explicit PlaylistModel(QObject* _parent = nullptr);

    int count() const;

    int rowCount(const QModelIndex& _parent = {}) const override;
    QVariant data(const QModelIndex& _index, int _role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void scanMusicFolder();

signals:
    void countChanged();

private:
    struct Track {
        QString title;
        QUrl url;
    };

    QList<Track> m_tracks;
};