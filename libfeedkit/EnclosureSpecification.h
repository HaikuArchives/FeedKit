#ifndef LIBFEEDKIT_ENCLOSURESPECIFICATION_H
#define LIBFEEDKIT_ENCLOSURESPECIFICATION_H

#include <vector>

#include <libfeedkit/Specification.h>

namespace FeedKit {
	class Enclosure;

	typedef std::vector<Enclosure *> EnclosureList;
	typedef Specification<Enclosure *> EnclosureSpecification;
};

#endif
