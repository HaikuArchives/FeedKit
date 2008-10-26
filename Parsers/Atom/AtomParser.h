#ifndef ATOMPARSER_H
#define ATOMPARSER_H

#include <libfeedkit/Channel.h>
#include <libfeedkit/Feed.h>
#include <libfeedkit/FeedParser.h>

#include <libxml/parser.h>

using namespace FeedKit;

class AtomParser : public FeedParser {
	public:
							AtomParser(void);
							~AtomParser(void);
							
		float				HandlesFeedURL(const char *url);
		float				HandlesFeed(const char *url, const char *feed, int32 length);

		Feed				*ParseFeed(const char *url, const char *feed, int32 length);
		
		const char			*Name(void);
		const char			*Description(void);
		
		BMessage			SettingsTemplate(void);
		BView				*SettingsView(BMessage settings);
		
	private:
		const char			*NodeContents(xmlNode * const node);
		const char			*ExtractChildNode(xmlNode * const parent, const char *name);

		status_t			ParseAtomFeed(Channel *channel, xmlNode *parent,
								bool convertLinks);
		status_t			ParseAtomEntry(Channel *channel, Item *item, xmlNode *node,
								bool convertLinks);

};

#endif
