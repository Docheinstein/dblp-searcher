#include "gui_element_publications.h"

QHash<int, QByteArray> GuiElementPublications::GUI_ELEMENT_PUBLICATION_ROLES = {
	{GuiElementPublicationRoleSerial, "serial"},
	{GuiElementPublicationRoleIdentifier, "identifier"},
	{GuiElementPublicationRoleRank, "rank"},
};

GuiElementPublications::GuiElementPublications(QObject *parent)
	: QAbstractListModel (parent) {}

void GuiElementPublications::addPublication(const GuiElementPublication &line)
{
	beginInsertRows(QModelIndex(), rowCount(), rowCount());
	mPublications.append(line);
	endInsertRows();
}

void GuiElementPublications::addPublications(const QVector<GuiElementPublication> &lines)
{
	beginInsertRows(QModelIndex(), rowCount(), rowCount() + lines.size() - 1);
	for (const GuiElementPublication& line : lines)
		mPublications.append(line);
	endInsertRows();
}

void GuiElementPublications::clearPublications()
{
	beginRemoveRows(QModelIndex(), 0, rowCount());
	mPublications.clear();
	endRemoveRows();
}

int GuiElementPublications::size()
{
	return mPublications.size();
}

int GuiElementPublications::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return mPublications.size();
}

QVariant GuiElementPublications::data(const QModelIndex &index, int role) const
{
	if (index.row() < 0 || index.row() >= mPublications.size())
		return QVariant();

	const GuiElementPublication &pub = mPublications.at(index.row());

	switch (role) {
	case GuiElementPublicationRoleSerial:
		return pub.serial();
	case GuiElementPublicationRoleIdentifier:
		return pub.identifier();
	case GuiElementPublicationRoleRank:
		return index.row() + 1;
	}

	return QVariant();
}

QHash<int, QByteArray> GuiElementPublications::roleNames() const
{
	return GUI_ELEMENT_PUBLICATION_ROLES;
}
