#include "query_types.h"

using QPET = QueryPublicationElementType;
using QPFT = QueryPublicationFieldType;

using QVET = QueryVenueElementType;
using QVFT = QueryVenueFieldType;

// BASE
QueryElementType::QueryElementType(const char *string) : StringType (string) {}
QueryFieldType::QueryFieldType(const char *string) : StringType (string) {}


// PUBLICATION

QPET * QPET::PUBLICATION = new QPET("publication");
QPET * QPET::ARTICLE = new QPET("article");
QPET * QPET::INCOLLECTION = new QPET("incollection");
QPET * QPET::INPROCEEDINGS = new QPET("inproc");
QPET * QPET::PHDTHESIS = new QPET("phThesis");
QPET * QPET::MASTERTHESIS = new QPET("masterThesis");


QPFT * QPFT::AUTHOR = new QPFT("author");
QPFT * QPFT::TITLE = new QPFT("title");
QPFT * QPFT::YEAR = new QPFT("year");

// VENUE

QVET * QVET::VENUE = new QVET("venue");

QVFT * QVFT::TITLE = new QVFT("title");
QVFT * QVFT::PUBLISHER = new QVFT("publisher");

