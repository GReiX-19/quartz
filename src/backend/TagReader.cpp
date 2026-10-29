#include "TagReader.hpp"

#include <QFile>
#include <QFileInfo>
#include <audioproperties.h>
#include <fileref.h>
#include <tag.h>

namespace {
    QString toQString(const TagLib::String& _s) {
        return QString::fromStdString(_s.to8Bit(true));
    }
}

TrackMetadata TagReader::read(const QString& _path) {
    const QFileInfo info(_path);

    TrackMetadata meta;
    meta.path = _path;
    meta.title = info.completeBaseName();
    meta.modified = info.lastModified();

    const TagLib::FileRef ref(QFile::encodeName(_path).constData(), true, TagLib::AudioProperties::Fast);

    if (ref.isNull())
        return meta;

    if (const TagLib::Tag* tag = ref.tag()) {
        const QString title = toQString(tag->title()).trimmed();
        if (!title.isEmpty())
            meta.title = title;
        meta.artist = toQString(tag->artist()).trimmed();
    }
    if (const TagLib::AudioProperties* props = ref.audioProperties()) {
        meta.durationMs = props->lengthInMilliseconds();
    }

    return meta;
}