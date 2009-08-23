#include "ParserManager.h"

#include <Directory.h>
#include <Entry.h>
#include <FindDirectory.h>
#include <Path.h>

#include <libfeedkit/Feed.h>
#include <libfeedkit/FeedParser.h>

#include <algorithm>
#include <stdio.h>

//#pragma mark Namespace import

using namespace FeedKit;

//#pragma mark SortParser

class SortParser {
	public:
		const char *url;
		const char *data;
		int32 length;
		
		SortParser(const char *_url, const char *_data, int32 _length) {
			url = _url;
			data = _data;
			length = _length;
		};
		
		bool operator() (FeedParser *lhs, FeedParser *rhs) {
			float lhsType = lhs->HandlesFeedURL(url);
			float rhsType = rhs->HandlesFeedURL(url);
			bool result = lhsType < rhsType;

			// Do a proper check if they're equal
			if (lhsType == rhsType) {
				result = lhs->HandlesFeed(url, data, length) < rhs->HandlesFeed(url, data, length);
			};
			
			return result;
		};
};


//#pragma mark Constants

const char *kAddonPath = "feed_kit/Parsers";

//#pragma mark Constructor

ParserManager::ParserManager(void) {
	BPath path;
	if (find_directory(B_USER_ADDONS_DIRECTORY, &path, true, NULL) == B_OK) {
		path.Append(kAddonPath);
		
		LoadAddons(path.Path());
	};
};

ParserManager::ParserManager(const char *path) {
	LoadAddons(path);
	
	BPath userPath;
	if (find_directory(B_USER_ADDONS_DIRECTORY, &userPath, true, NULL) == B_OK) {
		userPath.Append(kAddonPath);
		
		LoadAddons(userPath.Path());
	};
};

ParserManager::ParserManager(path_t paths) {
	int32 pathCount = paths.size();
	for (int32 i = 0; i < pathCount; i++) LoadAddons(paths[i].String());

	BPath userPath;
	if (find_directory(B_USER_ADDONS_DIRECTORY, &userPath, true, NULL) == B_OK) {
		userPath.Append(kAddonPath);
		
		LoadAddons(userPath.Path());
	};
};

ParserManager::~ParserManager(void) {
	parser_t::iterator pIt;
	for (pIt = 	fParsers.begin(); pIt != fParsers.end(); pIt++) {
		const char *name = pIt->second->Name();
		delete pIt->second;
		
		fprintf(stderr, "Unloading %s (%i): ", name, pIt->first);
		status_t unload = unload_add_on(pIt->first);
		fprintf(stderr, "%s\n", strerror(unload));
	};
};

//#pragma mark Public

Feed *ParserManager::Parse(const char *url, const char *data, int32 length) {
	Feed *feed = NULL;
	parser_t::iterator pIt;
	std::vector<FeedParser *> potential;

	for (pIt = fParsers.begin(); pIt != fParsers.end(); pIt++) {
		FeedParser *parser = pIt->second;
		if (parser->HandlesFeedURL(url) > 0.0f) potential.push_back(parser);
	};

	fprintf(stderr, "%i parsers claim to handle \"%s\"\n", potential.size(), url);

	if (potential.empty() == false) {
		if (potential.size() > 1) {
			sort(potential.begin(), potential.end(), SortParser(url, data, length));
		};

		int32 parsers = potential.size();
		for (int32 i = 0; i < parsers; i++) {
			FeedParser *parser = potential[i];
			// Verify the parser *really* handles the feed
			float ability = parser->HandlesFeed(url, data, length);

			fprintf(stderr, "\"%s\" (%i) claims %.0f%% handling ability\n", parser->Name(),
				i + 1, ability * 100);

			if (ability > 0.0f) {
				feed = parser->ParseFeed(url, data, length);

				if (feed != NULL) {
//					fprintf(stderr, "Parser %i (%s) of %i handled \"%s\"\n", i + 1, parser->Name(),
//						parsers, url);
					break;
				};
			};
		};
	};

	if (feed == false) fprintf(stderr, "No parser handled \"%s\"\n", url);
	
	return feed;
};

//#pragma mark Private

void ParserManager::LoadAddons(const char *path) {
	BDirectory dir(path);
	if (dir.InitCheck() != B_OK) return;

	entry_ref ref;
	while (dir.GetNextRef(&ref) == B_OK) {
		BPath addonPath(&ref);
		image_id image = load_add_on(addonPath.Path());
		FeedParser *(*instantiate)(BMessage);

		fprintf(stderr, "ParserManager::LoadAddons(%s): Loading \"%s\"... %s\n", path,
			addonPath.Leaf(), image < B_OK ? strerror(image) : "Done");

		if (image >= B_OK) {
			status_t status = get_image_symbol(image, "instantiate_parser", B_SYMBOL_TYPE_TEXT,
				(void **)&instantiate);
		
			fprintf(stderr, "\tReading instantiate_parser: %s (%i)\n", strerror(status), status);
			if (status == B_OK) {
			
				BMessage settings;
				fParsers[image] = instantiate(settings);
			} else {
				unload_add_on(image);
			};
		} else {
			unload_add_on(image);
		};
	};
};
