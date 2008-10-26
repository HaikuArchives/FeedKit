#ifndef PARSERMANAGER_H
#define PARSERMANAGER_H

#include <image.h>
#include <String.h>

#include <vector>
#include <map>

#include <libfeedkit/FeedKitConstants.h>

namespace FeedKit {
	class Feed;
	class FeedParser;
};

typedef vector<BString> path_t;
typedef map<image_id, FeedKit::FeedParser *> parser_t;

class ParserManager {
	public:
							ParserManager(void);
							ParserManager(const char *path);
							ParserManager(path_t paths);
							~ParserManager(void);
							
		FeedKit::Feed		*Parse(const char *url, const char *data, int32 length);
	
	private:
		void				LoadAddons(const char *path);
	
		parser_t			fParsers;		
};

#endif

