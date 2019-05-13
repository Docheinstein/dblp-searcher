#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <limits>

// Whether precompute all the ief (slow init, but faster searches) or not
#define LAZY_IEF

namespace Config {
	namespace Index {
		namespace Extensions {
			const QString IDENTIFIERS = ".idix";
			const QString POSTING_LIST = ".plix";
			const QString VOCABULARY = ".vix";
			const QString ELEMENTS_POS = ".epix";
			const QString CROSSREFS = ".cix";
		}
		namespace PostingList {
			const quint32 ELEMENT_SERIAL_BITS = 23u;
			const quint32 ELEMENT_SERIAL_THRESHOLD = 1u << ELEMENT_SERIAL_BITS; // 8388608

			const quint32 FIELD_NUM_BITS = 9u;
			const quint32 FIELD_NUM_THRESHOLD = 1u << FIELD_NUM_BITS; // 512

			const quint32 IN_FIELD_POS_BITS = 8u;
			const quint32 IN_FIELD_POS_THRESHOLD = 1u << IN_FIELD_POS_BITS; // 256

			const quint32 POST_BITS = ELEMENT_SERIAL_BITS + FIELD_NUM_BITS + IN_FIELD_POS_BITS;
			const quint32 POST_BYTES = POST_BITS / 8;
		}
		namespace Vocabulary {
			const quint32 REF_SHRINKED_BITS = 15u;
			const quint32 REF_SHRINKED_THRESHOLD = 1u << REF_SHRINKED_BITS;
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

		namespace Config {
//			const int POSTING_LIST_TERM_REFERENCE_SIZE = 32u;
//			const int POSTING_LIST_TERM_REFERENCE_BYTE_SIZE =
//					POSTING_LIST_TERM_REFERENCE_SIZE / 8;

//			const int POST_ELEMENT_ID_MAX_2POW = 23u; // max ~7KK elements

//			const int POST_FIELD_NUMBER_MAX_2POW = 4u; // max 8 field of the same type
//			const int POST_FIELD_NUMBER_MASK =
//					static_cast<quint32>(~0u) >>
//							(POSTING_LIST_TERM_REFERENCE_SIZE - POST_FIELD_NUMBER_MAX_2POW);

//			const int POST_IN_FIELD_TERM_POS_MAX_2POW = 5u; // max 32 terms for field
//			const int POST_IN_FIELD_TERM_POS_MASK =
//					static_cast<quint32>(~0u) >>
//							(POSTING_LIST_TERM_REFERENCE_SIZE - POST_IN_FIELD_TERM_POS_MAX_2POW);

//			const int POST_ELEMENT_ID_MAX =
//					~0u >> (POSTING_LIST_TERM_REFERENCE_SIZE - POST_ELEMENT_ID_MAX_2POW);
//			const int POST_FIELD_NUMBER_MAX =
//					~0u >> (POSTING_LIST_TERM_REFERENCE_SIZE - POST_FIELD_NUMBER_MAX_2POW);
//			const int POST_IN_FIELD_TERM_POS_MAX =
//					~0u >> (POSTING_LIST_TERM_REFERENCE_SIZE - POST_IN_FIELD_TERM_POS_MAX_2POW);

//			const int VOCAB_FIELD_POSTS_THRESHOLD =
//					static_cast<quint16>(~0u) >> 1; // 32767

//			const quint32 VOCAB_FIELD_POSTS_ABOVE_THRESHOLD_FLAG =
//					1u << 31; // left bit of 32bits
		}
	}
}

namespace PostingListConf = Config::Index::PostingList;
namespace VocabularyConf = Config::Index::Vocabulary;

#endif // CONFIG_H
