#ifndef XML_TYPES_H
#define XML_TYPES_H

#include <QString>
#include <commons/adt/type/string_type.h>

// TODO typedef XmlElementCase

class XmlElementType : public StringType {
public:
	XmlElementType(const char *s) : StringType(s) {}

	static XmlElementType *ARTICLE;
	static XmlElementType *INCOLLECTION;
	static XmlElementType *BOOK;
	static XmlElementType *INPROCEEDINGS;
	static XmlElementType *PROCEEDINGS;
	static XmlElementType *PHDTHESIS;
	static XmlElementType *MASTERTHESIS;
};


class XmlFieldType : public StringType {
public:
	XmlFieldType(const char *s) : StringType(s) {}

	static XmlFieldType *AUTHOR;
	static XmlFieldType *TITLE;
	static XmlFieldType *YEAR;
	static XmlFieldType *JOURNAL;
	static XmlFieldType *PUBLISHER;
	static XmlFieldType *BOOKTITLE;
	static XmlFieldType *CROSSREF;
};

class XmlAttributeType : public StringType {
public:
	XmlAttributeType(const char *s) : StringType(s) {}

	static XmlAttributeType *KEY;
};

#endif // XML_TYPES_H
