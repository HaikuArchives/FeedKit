#ifndef RSSPARSER_H
#define RSSPARSER_H

#include <libfeedkit/Channel.h>
#include <libfeedkit/Feed.h>
#include <libfeedkit/FeedParser.h>

#include <libxml/parser.h>

using namespace FeedKit;

class RSSParser : public FeedParser {
	public:
							RSSParser(void);
							~RSSParser(void);
							
		float				HandlesFeedURL(const char *url);
		float				HandlesFeed(const char *url, const char *feed, int32 length);

		Feed				*ParseFeed(const char *url, const char *feed, int32 length);
		
		const char			*Name(void);
		const char			*Description(void);
		
	private:
		BString				NodeContents(xmlNode *node);

		void				ExtractChannelNodeDetails(Channel *channel, xmlNode *node);
		void				ExtractItemNodeDetails(Item *item, xmlNode *node);
		Enclosure			*ExtractMediaContent(xmlNode *node);
};

#endif
