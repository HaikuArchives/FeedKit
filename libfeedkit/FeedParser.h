#ifndef LIBFEEDKIT_FEEDPARSER_H
#define LIBFEEDKIT_FEEDPARSER_H

#include <Message.h>
#include <View.h>

#include <libfeedkit/FeedKitConstants.h>

namespace FeedKit {
	class Feed;
	
	class FeedParser {
		public:
								FeedParser(void);
			virtual				~FeedParser(void);
			
			// This will get passed the URL of the feed. Based on this give a rough estimation of the
			//	ability to handle the feed. Eg. an RSS parser would return 0.0 (unsuported)
			//	for a ".atom" feed. Whilst it might return 0.5 (sort of supported) for a ".xml"
			//	and 1.0 for a ".rss" feed
			// Return 0.0 (unsupported) to 1.0 (supported). Eg
			virtual float		HandlesFeedURL(const char *url) = 0;
	
			// This will get passed the entire feed contents. This will get used if multiple parsers
			//	return the same value for HandlesFeedType. This allows you to give a more informed
			//	decision of your parsing ability based on the content of the feed. Return 0.0 - 1.0
			virtual float		HandlesFeed(const char *url, const char *feed, int32 length) = 0;
			
			// Again, the entire feed. Given the feed you must parse it and map its contents into a
			//	Feed object. If for some reason you cannot parse the feed (ie. it's invalid) return
			//	NULL
			virtual Feed 		*ParseFeed(const char *url, const char *feed, int32 length) = 0;
	
			// Descriptive stuff for User-Interface purposes
			virtual const char	*Name(void) = 0;
			virtual const char	*Description(void) = 0;
				
			// Parses an URL into its components - returns B_OK if everything parsed okay
			//	Given "http://beos.bong.com.au/feeds/news/main.xml" you'd get;
			//		protocol = "http"
			//		domain = "beos.bong.com.au"
			//		path = "feeds/main/"
			//		file = "news.xml"
			status_t			ParseURL(const char *url, BString &protocol, BString &domain,
									BString &path, BString &file);
		private:
	};
	
	extern "C" {
		// Your addon must export this so that the Feed Server can load an instance of the parser
		FeedParser *instantiate_parser(BMessage settings);
	};
};

#endif
