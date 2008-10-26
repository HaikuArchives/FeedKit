#ifndef LIBFEEDKIT_CHANNELSPECIFICATION_H
#define LIBFEEDKIT_CHANNELSPECIFICATION_H

#include <libfeedkit/Specification.h>

namespace FeedKit {
	class Channel;	
	
	typedef vector<Channel *> ChannelList;
	typedef Specification<Channel *> ChannelSpecification;
};

#endif
