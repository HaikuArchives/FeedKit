#include "ObjectCache.h"

#include "Feed.h"
#include "Channel.h"
#include "Item.h"
#include "Enclosure.h"
#include "Content.h"

#include <map>

//#pragma mark Namespace import

using namespace FeedKit;
using namespace FeedKit::Private;

//#pragma mark Typedefs

typedef map<BString, Feed *> cache_feed_t;
typedef map<BString, Channel *> cache_channel_t;
typedef map<BString, Item *> cache_item_t;
typedef map<BString, Enclosure *> cache_enclosure_t;
typedef map<BString, Content *> cache_content_t;

//#pragma mark CacheHelper - Declaration

namespace FeedKit {
	namespace Private {
		class CacheHelper {
			public:
									CacheHelper(void);
									~CacheHelper(void);

				void				AddFeed(Feed *feed);
				void				Clear(void);
				
				Feed				*GetFeed(const char *id) const;
				Channel				*GetChannel(const char *id) const;
				Item				*GetItem(const char *id) const;
				Enclosure			*GetEnclosure(const char *id) const;
				Content				*GetContent(const char *id) const;

			private:
				void				ClearFeed(Feed *feed);
			
				cache_feed_t		fFeed;
				cache_channel_t		fChannel;
				cache_item_t		fItem;
				cache_enclosure_t	fEnclosure;
				cache_content_t		fContent;
		};
	};
};

//#pragma mark Cache Helper - Implementation

CacheHelper::CacheHelper(void) {
};

CacheHelper::~CacheHelper(void) {
	fFeed.clear();
	fChannel.clear();
	fItem.clear();
	fEnclosure.clear();
	fContent.clear();
};

void CacheHelper::AddFeed(Feed *feed) {
	ClearFeed(feed);

	fFeed[feed->UUID()] = feed;
	
	for (uint32 i = 0; i < feed->ChannelCount(); i++) {
		Channel *channel = feed->ChannelAt(i);
		fChannel[channel->UUID()] = channel;
		
		for (uint32 j = 0; j < channel->ItemCount(); j++) {
			Item *item = channel->ItemAt(j);
			fItem[item->UUID()] = item;

			for (uint32 k = 0; k < item->ContentCount(); k++) {
				Content *content = item->ContentAt(k);
				fContent[content->UUID()] = content;
			};
			
			for (uint32 k = 0; k < item->EnclosureCount(); k++) {
				Enclosure *enclosure = item->EnclosureAt(k);
				fEnclosure[enclosure->UUID()] = enclosure;
			};
		}
	};
};

void CacheHelper::Clear(void) {
	fFeed.clear();
	fChannel.clear();
	fItem.clear();
	fContent.clear();
	fEnclosure.clear();
};
				
Feed *CacheHelper::GetFeed(const char *id) const {
	Feed *feed = NULL;
	cache_feed_t::const_iterator fIt = fFeed.find(id);
	if (fIt != fFeed.end()) feed = fIt->second;
	
	return feed;
};

Channel *CacheHelper::GetChannel(const char *id) const {
	Channel *channel = NULL;
	cache_channel_t::const_iterator cIt = fChannel.find(id);
	if (cIt != fChannel.end()) channel = cIt->second;
	
	return channel;
};

Item *CacheHelper::GetItem(const char *id) const {
	Item *item = NULL;
	cache_item_t::const_iterator iIt = fItem.find(id);
	if (iIt != fItem.end()) item = iIt->second;
	
	return item;
};

Enclosure *CacheHelper::GetEnclosure(const char *id) const {
	Enclosure *enclosure = NULL;
	cache_enclosure_t::const_iterator eIt = fEnclosure.find(id);
	if (eIt != fEnclosure.end()) enclosure = eIt->second;
	
	return enclosure;
};

Content *CacheHelper::GetContent(const char *id) const {
	Content *content = NULL;
	cache_content_t::const_iterator cIt = fContent.find(id);
	if (cIt != fContent.end()) content = cIt->second;
	
	return content;
};

void CacheHelper::ClearFeed(Feed *feed) {
	cache_feed_t::iterator fIt = fFeed.find(feed->UUID());
	if (fIt != fFeed.end()) {
		fFeed.erase(fIt);
		
		for (uint32 i = 0; i < feed->ChannelCount(); i++) {
			Channel *channel = feed->ChannelAt(i);
			cache_channel_t::iterator cIt = fChannel.find(channel->UUID());
			
			if (cIt == fChannel.end()) continue;
			
			fChannel.erase(cIt);
			
			for (uint32 j = 0; j < channel->ItemCount(); j++) {
				Item *item = channel->ItemAt(j);
				cache_item_t::iterator iIt = fItem.find(item->UUID());
				
				if (iIt == fItem.end()) continue;
				
				fItem.erase(iIt);
				
				for (uint32 k = 0; k < item->ContentCount(); k++) {
					Content *content = item->ContentAt(k);
					cache_content_t::iterator coIt = fContent.find(content->UUID());
					
					if (coIt != fContent.end()) fContent.erase(coIt);
				};
				
				for (uint32 k = 0; k < item->EnclosureCount(); k++) {
					Enclosure *enclosure = item->EnclosureAt(k);
					cache_enclosure_t::iterator eIt = fEnclosure.find(enclosure->UUID());
					
					if (eIt != fEnclosure.end()) fEnclosure.erase(eIt);
				};
			}
		};
	};
};

//#pragma mark Constructor

ObjectCache::ObjectCache(void)
	: fCacheHelper(NULL) {
	
	fCacheHelper = new CacheHelper();
};

ObjectCache::~ObjectCache(void) {
	delete fCacheHelper;
};

//#pragma mark Public

void ObjectCache::AddFeed(Feed *feed) {
	fCacheHelper->AddFeed(feed);
};

void ObjectCache::Clear(void) {
	fCacheHelper->Clear();
};

Feed *ObjectCache::GetFeed(const char *id) const {
	return fCacheHelper->GetFeed(id);
};

Channel *ObjectCache::GetChannel(const char *id) const {
	return fCacheHelper->GetChannel(id);
};

Item *ObjectCache::GetItem(const char *id) const {
	return fCacheHelper->GetItem(id);
};

Enclosure *ObjectCache::GetEnclosure(const char *id) const {
	return fCacheHelper->GetEnclosure(id);
};

Content *ObjectCache::GetContent(const char *id) const {
	return fCacheHelper->GetContent(id);
};

