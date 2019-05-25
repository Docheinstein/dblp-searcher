#ifndef DBLP_DEFS_H
#define DBLP_DEFS_H

#include <QtGlobal>

// Definitions of the sizes used in the entire application of the index related stuff.
// The maximum have been figured out by doing some reverse engineering of
// the content of dblp.xml; it's a miracle that we can handle everything within
// 40 bit since every bit is mandatory for handle all the possibible elements
// fields num and term pos of the xml file.

typedef quint32 elem_serial;	// Max 2^23
typedef quint16 field_num;		// Max 2^9
typedef quint8 term_pos;		// Max 2^8

typedef quint32 elem_pos;		// Max 2^32
								// This means that if dblp.xml will ever be
								// more than ~4GB we'll have to re structure everything


#endif // DBLP_DEFS_H
