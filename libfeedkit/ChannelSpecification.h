#ifndef LIBFEEDKIT_CHANNELSPECIFICATION_H
#define LIBFEEDKIT_CHANNELSPECIFICATION_H

#include <vector>

#include <libfeedkit/Specification.h>

namespace FeedKit {
	class Channel;	
	
	typedef std::vector<Channel *> ChannelList;
	typedef Specification<Channel *> ChannelSpecification;
};

#endif
