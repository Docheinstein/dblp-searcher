#include "xml_types.h"

XmlElementType * XmlElementType::ARTICLE = new XmlElementType("article");
XmlElementType * XmlElementType::INCOLLECTION = new XmlElementType("incollection");
XmlElementType * XmlElementType::BOOK = new XmlElementType("book");
XmlElementType * XmlElementType::INPROCEEDINGS = new XmlElementType("inproceedings");
XmlElementType * XmlElementType::PROCEEDINGS = new XmlElementType("proceedings");
XmlElementType * XmlElementType::PHDTHESIS = new XmlElementType("phdthesis");
XmlElementType * XmlElementType::MASTERTHESIS = new XmlElementType("masterthesis");

XmlFieldType * XmlFieldType::AUTHOR = new XmlFieldType("author");
XmlFieldType * XmlFieldType::TITLE = new XmlFieldType("title");
XmlFieldType * XmlFieldType::YEAR = new XmlFieldType("year");
XmlFieldType * XmlFieldType::JOURNAL = new XmlFieldType("journal");
XmlFieldType * XmlFieldType::PUBLISHER = new XmlFieldType("publisher");
XmlFieldType * XmlFieldType::BOOKTITLE = new XmlFieldType("booktitle");
XmlFieldType * XmlFieldType::CROSSREF = new XmlFieldType("crossref");

XmlAttributeType * XmlAttributeType::KEY = new XmlAttributeType("key");
