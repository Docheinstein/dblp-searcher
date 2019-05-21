#ifndef GUI_ELEMENT_PUBLICATIONS_H
#define GUI_ELEMENT_PUBLICATIONS_H

#include <QAbstractListModel>

#include <gui/models/element_publication/gui_element_publication.h>

class GuiElementPublications : public QAbstractListModel {

	Q_OBJECT

public:
	GuiElementPublications(QObject *parent = nullptr);

	void addPublication(const GuiElementPublication &line);
	void addPublications(const QVector<GuiElementPublication> &line);
	void clearPublications();

	int size() const;

	// QAbstractItemModel interface
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QHash<int, QByteArray> roleNames() const;

private:
	enum GuiElementPublicationRole {
		GuiElementPublicationRoleSerial = Qt::UserRole + 1,
		GuiElementPublicationRoleIdentifier,
		GuiElementPublicationRoleRank,
	};

	static QHash<int, QByteArray> GUI_ELEMENT_PUBLICATION_ROLES;

	QVector<GuiElementPublication> mPublications;
};

#endif // GUI_ELEMENT_PUBLICATIONS_H
