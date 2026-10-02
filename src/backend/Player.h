#pragma once

#include <QObject>
#include <QtQml/qqmlregistration.h>

class Player : public QObject {
    Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(bool playing READ playing NOTIFY playingChanged)

public:
    explicit Player(QObject* _parent = nullptr);

    bool playing() const;

    Q_INVOKABLE void toggle();

signals:
    void playingChanged();

private:
    bool m_playing = false;
};