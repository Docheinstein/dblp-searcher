#ifndef GUI_QUERY_MATCHES_H
#define GUI_QUERY_MATCHES_H

#include <QAbstractListModel>
#include "dblp/query/match/query_match.h"
#include "gui/models/query_match/gui_query_match.h"

class IRModel;

class GuiQueryMatches : public QAbstractListModel {

	Q_OBJECT

public:
	GuiQueryMatches(QObject *parent = nullptr);

	void addMatch(const QueryMatch &match);
	void clearMatches();

	void setIrModel(IRModel *irModel);

	int size();

	// QAbstractItemModel interface
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QHash<int, QByteArray> roleNames() const;

private:
	enum GuiQueryMatchRole {
		GuiQueryMatchRolePublicationIdentifier = Qt::UserRole + 1,
		GuiQueryMatchRolePublicationElementType,
		GuiQueryMatchRoleVenueIdentifier,
		GuiQueryMatchRoleVenueElementType,
		GuiQueryMatchRoleType,
		GuiQueryMatchRoleScore,
		GuiQueryMatchRoleRank,
	};

	static QHash<int, QByteArray> GUI_QUERY_MATCH_ROLES;

	QList<GuiQueryMatch> mMatches;

	IRModel *mIrModel;
};

#endif // GUI_QUERY_MATCHES_H
