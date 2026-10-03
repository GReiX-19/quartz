#pragma once

#include <QSortFilterProxyModel>
#include <QString>
#include <QtQml/qqmlregistration.h>

class TrackFilterModel : public QSortFilterProxyModel {
    Q_OBJECT;
    QML_ELEMENT;
    Q_PROPERTY(QString filterText READ filterText WRITE setFilterText NOTIFY filterTextChanged);

public:
    explicit TrackFilterModel(QObject* _parent = nullptr);

    QString filterText() const;
    void setFilterText(const QString& _text);

signals:
    void filterTextChanged();

private:
    QString m_filterText;
};