#include "gui_query_matches.h"
#include "dblp/irmodel/base/ir_model.h"

#define NOT_FOUND -1

QHash<int, QByteArray> GuiQueryMatches::GUI_QUERY_MATCH_ROLES = {
	{GuiQueryMatchRolePublicationSerial, "publicationSerial"},
	{GuiQueryMatchRolePublicationIdentifier, "publicationIdentifier"},
	{GuiQueryMatchRolePublicationElementType, "publicationElementType"},

	{GuiQueryMatchRoleVenueSerial, "venueSerial"},
	{GuiQueryMatchRoleVenueIdentifier, "venueIdentifier"},
	{GuiQueryMatchRoleVenueElementType, "venueElementType"},

	{GuiQueryMatchRoleType, "type"},
	{GuiQueryMatchRoleScore, "score"},
	{GuiQueryMatchRoleRank, "rank"},
};

GuiQueryMatches::GuiQueryMatches(QObject *parent) : QAbstractListModel (parent)
{

}

QVector<GuiQueryMatch> GuiQueryMatches::matches()
{
	return mMatches;
}

void GuiQueryMatches::addMatch(const QueryMatch &match)
{
	beginInsertRows(QModelIndex(), rowCount(), rowCount());
	mMatches.append(GuiQueryMatch(match));
	endInsertRows();
}

void GuiQueryMatches::addMatches(const QVector<QueryMatch> &matches)
{
	beginInsertRows(QModelIndex(), rowCount(), rowCount() + matches.size() - 1);
	for (const QueryMatch& match : matches) {
		mMatches.append(GuiQueryMatch(match));
	}
	endInsertRows();
}

void GuiQueryMatches::clearMatches()
{
	beginRemoveRows(QModelIndex(), 0, rowCount());
	mMatches.clear();
	endRemoveRows();
}


void GuiQueryMatches::setIrModel(IRModel *irModel)
{
	mIrModel = irModel;
}

int GuiQueryMatches::size()
{
	return mMatches.size();
}

int GuiQueryMatches::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return mMatches.size();
}

QVariant GuiQueryMatches::data(const QModelIndex &index, int role) const
{
	if (index.row() < 0 || index.row() >= mMatches.size())
		return QVariant();

	const QueryMatch &match = mMatches.at(index.row()).model();

	switch (role) {
	case GuiQueryMatchRolePublicationSerial:
		if (match.matchType() == QueryMatchType::Publication ||
			match.matchType() == QueryMatchType::PublicationVenue) {
			return match.publication().elementSerial();
		}
		return NOT_FOUND;
	case GuiQueryMatchRolePublicationIdentifier: {
		// Retrieve the identifier based on the serial
		QString identifier;
		if (match.matchType() == QueryMatchType::Publication ||
			match.matchType() == QueryMatchType::PublicationVenue) {
			if (mIrModel->index().identifier(
					match.publication().elementSerial(), identifier))
				return identifier;
		}
		return "<unknown>";
	}
	case GuiQueryMatchRolePublicationElementType:
		if (match.matchType() == QueryMatchType::Publication ||
			match.matchType() == QueryMatchType::PublicationVenue) {
			return elementTypeString(match.publication().elementType());
		}
		return "<unknown>";
	case GuiQueryMatchRoleVenueSerial:
		if (match.matchType() == QueryMatchType::Venue ||
			match.matchType() == QueryMatchType::PublicationVenue) {
			return match.venue().elementSerial();
		}
		return NOT_FOUND;
	case GuiQueryMatchRoleVenueIdentifier: {
		// Retrieve the identifier based on the serial
		QString identifier;
		if (match.matchType() == QueryMatchType::Venue ||
			match.matchType() == QueryMatchType::PublicationVenue) {
			if (mIrModel->index().identifier(
					match.venue().elementSerial(), identifier))
				return identifier;
		}
		return "<unknown>";
	}
	case GuiQueryMatchRoleVenueElementType:
		if (match.matchType() == QueryMatchType::Venue ||
			match.matchType() == QueryMatchType::PublicationVenue) {
			return elementTypeString(match.venue().elementType());
		}
		return "<unknown>";
	case GuiQueryMatchRoleType:
		switch (match.matchType()) {
		case QueryMatchType::Publication:
			return GuiQueryMatchType::Publication;
		case QueryMatchType::Venue:
			return GuiQueryMatchType::Venue;
		case QueryMatchType::PublicationVenue:
			return GuiQueryMatchType::PublicationVenue;
		}
	case GuiQueryMatchRoleScore:
		return match.score();
	case GuiQueryMatchRoleRank:
		return index.row() + 1;
	}

	return QVariant();
}

QHash<int, QByteArray> GuiQueryMatches::roleNames() const
{
	return GUI_QUERY_MATCH_ROLES;
}
