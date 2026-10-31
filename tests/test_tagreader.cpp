#include "TagReader.hpp"

#include <QFile>
#include <QTemporaryDir>
#include <QtTest>

class TagReaderTest : public QObject {
    Q_OBJECT;

private:
    static QString fixture(const QString& _name) {
        return QString::fromUtf8(TEST_DATA_DIR) + "/" + _name;
    }

private slots:
    void readsTags() {
        const TrackMetadata m = TagReader::read(fixture("tagged.mp3"));
        QCOMPARE(m.title, QStringLiteral("Test Title"));
        QCOMPARE(m.artist, QStringLiteral("Test Artist"));
        QVERIFY(qAbs(m.durationMs - 1000) < 100);
        QVERIFY(m.modified.isValid());
    }

    void fallsBackToFilenameWithoutTags() {
        const TrackMetadata m = TagReader::read(fixture("untagged.mp3"));
        QCOMPARE(m.title, QStringLiteral("untagged"));
        QVERIFY(m.artist.isEmpty());
    }

    void survivesGarbageFile() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath("not_audio.mp3");
        QFile f(path);
        QVERIFY(f.open(QIODevice::WriteOnly));
        f.write("definitely not audio");
        f.close();

        const TrackMetadata m = TagReader::read(path);
        QCOMPARE(m.title, QStringLiteral("not_audio"));
        QVERIFY(m.artist.isEmpty());
        QCOMPARE(m.durationMs, qint64(0));
    }

    void survivesMissingFiles() {
        const TrackMetadata m = TagReader::read("/no/such/file.mp3");
        QCOMPARE(m.title, QStringLiteral("file"));
        QCOMPARE(m.durationMs, qint64(0));
    }
};

QTEST_GUILESS_MAIN(TagReaderTest)
#include "test_tagreader.moc"