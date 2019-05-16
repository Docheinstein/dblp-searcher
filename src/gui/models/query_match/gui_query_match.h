#ifndef GUI_QUERY_MATCH_H
#define GUI_QUERY_MATCH_H

#include <dblp/query/match/query_match.h>

#define GUI_QUERY_MATCH_TYPE_QML_NAME "QueryMatchType"

class GuiQueryMatchType : public QObject {
	Q_OBJECT

public:
	enum Type {
		Publication,
		Venue,
		PublicationVenue
	};
	Q_ENUM(Type)
};

class GuiQueryMatch {

public:
	GuiQueryMatch();
	GuiQueryMatch(const QueryMatch &match);

	QueryMatch model() const;

private:
	QueryMatch mModel;
};

#endif // GUI_QUERY_MATCH_H
