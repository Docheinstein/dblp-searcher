#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <limits>

namespace Config {
	namespace Index {
		namespace Extensions {
			const QString IDENTIFIERS = ".idix";
			const QString POSTING_LIST = ".plix";
			const QString VOCABULARY = ".vix";
			const QString XML_POS = ".xpix";
			const QString CROSSREFS = ".cix";
		}

		namespace PostingList {
			const quint32 ELEMENT_SERIAL_BITS = 23u;
			const quint32 ELEMENT_SERIAL_THRESHOLD = 1u << ELEMENT_SERIAL_BITS; // 8388608

			const quint32 FIELD_NUM_BITS = 9u;
			const quint32 FIELD_NUM_THRESHOLD = 1u << FIELD_NUM_BITS; // 512

			const quint32 IN_FIELD_POS_BITS = 8u;
			const quint32 IN_FIELD_POS_THRESHOLD = 1u << IN_FIELD_POS_BITS; // 256

			const quint32 POST_BITS_MONOFIELD = 32;
			const quint32 POST_BYTES_MONOFIELD = POST_BITS_MONOFIELD / 8; // 4

			const quint32 POST_BITS_MULTIFIELD = 40;
			const quint32 POST_BYTES_MULTIFIELD = POST_BITS_MULTIFIELD / 8; // 5
		}

		namespace Vocabulary {
			const quint32 REF_SHRINKED_BITS = 15u;
			const quint32 REF_SHRINKED_THRESHOLD = 1u << REF_SHRINKED_BITS; // 3276832767
			const quint32 REF_SHRINKED_FLAG = 1u << REF_SHRINKED_BITS;

			const quint32 REF_EXTENDEND_BITS = 31;
			const quint32 REF_EXTENDED_THRESHOLD = 1u << REF_EXTENDEND_BITS;
			const quint32 REF_EXTENDED_FLAG = 1u << REF_EXTENDEND_BITS;
		}

		namespace ElementsPosition {
			const qint64 ELEMENT_POS_BITS = 32;
			const qint64 ELEMENT_POS_BYTES = ELEMENT_POS_BITS / 8;
			const qint64 ELEMENT_POS_THRESHOLD = 1l << ELEMENT_POS_BITS;
		}
	}
}

#endif // CONFIG_H
