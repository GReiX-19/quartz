#include "TrackFilterModel.hpp"
#include "PlaylistModel.hpp"

TrackFilterModel::TrackFilterModel(QObject* _parent)
    : QSortFilterProxyModel(_parent) {

    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setFilterRole(PlaylistModel::TitleRole);
}

QString TrackFilterModel::filterText() const {
    return m_filterText;
}

void TrackFilterModel::setFilterText(const QString& _text) {
    if (m_filterText == _text)
        return;

    m_filterText = _text;
    setFilterFixedString(_text);

    emit filterTextChanged();
}