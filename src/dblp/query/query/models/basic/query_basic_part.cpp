#include "query_basic_part.h"
#include "dblp/shared/element_field_type/element_field_type.h"

ElementFieldTypes QueryBasicPart::elementFieldTypes()
{
	return ElementFieldType::All; // no filter
}

QueryElementType *QueryBasicPart::element()
{
	return nullptr; // no element
}

QueryFieldType *QueryBasicPart::field()
{
	return nullptr; // no field
}
