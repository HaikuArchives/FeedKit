#ifndef LIBFEEDKIT_ENCLOSURESPECIFICATION_H
#define LIBFEEDKIT_ENCLOSURESPECIFICATION_H

#include <libfeedkit/Specification.h>

namespace FeedKit {
	class Enclosure;

	typedef vector<Enclosure *> EnclosureList;
	typedef Specification<Enclosure *> EnclosureSpecification;
};

#endif
