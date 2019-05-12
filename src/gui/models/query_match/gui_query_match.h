#ifndef GUI_QUERY_MATCH_H
#define GUI_QUERY_MATCH_H

#include <dblp/query/match/query_match.h>

class GuiQueryMatch {

public:
	GuiQueryMatch();
	GuiQueryMatch(const QueryMatch &match);

	QueryMatch model() const;

private:
	QueryMatch mModel;
};

#endif // GUI_QUERY_MATCH_H
