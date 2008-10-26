#ifndef LIBFEEDKIT_FEEDSPECIFICATION_H
#define LIBFEEDKIT_FEEDSPECIFICATION_H

#include <libfeedkit/Specification.h>

namespace FeedKit {
	class Feed;
	
	typedef vector<Feed *> FeedList;
	typedef Specification<Feed *> FeedSpecification;
};

#endif
