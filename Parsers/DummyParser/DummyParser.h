#ifndef DUMMYPARSER_H
#define DUMMYPARSER_H

#include <libfeedkit/FeedParser.h>

class DummyParser : public FeedParser {
	public:
							DummyParser(void);
							~DummyParser(void);
							
		float				HandlesFeedType(const char *feedtype);
		float				HandlesFeed(const char *feed, int32 length);
		Channel				*ParseFeed(const char *feed, int32 length);
		const char			*Name(void);
	private:
};

#endif
