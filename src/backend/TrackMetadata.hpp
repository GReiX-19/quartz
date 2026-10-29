#pragma once

#include <QDateTime>
#include <QString>

struct TrackMetadata {
    QString path;
    QString title;
    QString artist;
    qint64 durationMs = 0;
    QDateTime modified;
};