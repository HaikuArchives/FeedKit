#ifndef LIBFEEDKIT_OBJECTCACHE_H
#define LIBFEEDKIT_OBJECTCACHE_H

#include <String.h>

namespace FeedKit {
	class Channel;
	class Enclosure;
	class Feed;
	class Item;
	class Content;

	namespace Private {
		class CacheHelper;
	};
	
	using namespace Private;

	class ObjectCache {
		public:
								ObjectCache(void);
			virtual				~ObjectCache(void);
		
			// Public
			void				AddFeed(Feed *feed);
			void				Clear(void);
			
			Feed				*GetFeed(const char *id) const;
			Channel				*GetChannel(const char *id) const;
			Item				*GetItem(const char *id) const;
			Enclosure			*GetEnclosure(const char *id) const;
			Content				*GetContent(const char *id) const;

		private:
			CacheHelper			*fCacheHelper;
	};
};

#endif
