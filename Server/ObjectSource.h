#ifndef OBJECTSOURCE_H
#define OBJECTSOURCE_H

namespace FeedKit {
	class Feed;
	class Channel;
	class Item;
	class Enclosure;
};

class ObjectSource {
	public:
	
		virtual FeedKit::Feed		*GetFeed(const char *uuid) = 0;
		virtual FeedKit::Channel	*GetChannel(const char *uuid) = 0;
		virtual FeedKit::Item		*GetItem(const char *uuid) = 0;
		virtual FeedKit::Enclosure	*GetEnclosure(const char *uuid) = 0;

};

#endif
