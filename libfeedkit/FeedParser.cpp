#include "FeedParser.h"
#include "RegEx.h"

#include <stdio.h>

//#pragma mark Namespace import

using namespace FeedKit;

//#pragma mark Constants

//#pragma mark Constructor

FeedParser::FeedParser(void) {
};

FeedParser::~FeedParser(void) {
};

//#pragma mark public

status_t FeedParser::ParseURL(const char *url, BString &protocol, BString &domain, BString &path,
	BString &file) {
	
	printf("FeedParser::ParseURL(%s)\n", url);
	
	status_t status = B_ERROR;
	
	RegEx *regURL = NULL;
	try {
//		regURL = new RegEx(argv[1], PCRE_ANCHORED);
//		regURL = new RegEx("(?:([\\w]+?)://)([\\w\\.]+?)/(?:([\\w\\/\\.]+?)\\/$)([\\w\\.\\?\\&]+?)", 0);
//		regURL = new RegEx("(?:([\\w]+?)://)([\\w\\.]+?)/(?:([\\w\\/\\.]+?))/([\\w\\/\\?\\&]+?)", PCRE_ANCHORED);
//		regURL = new RegEx("(([\\w]+)://)([\\w\\.]+)", 0);
//"^(([\\w]+?):\\/\\/?([^:\\/\\s]+)((\\/\\w+)*\\/)([\\w\\-\\.]+[^#?\\s]+)(.*)?(#[\\w\\-]+))$", PCRE_ANCHORED);
regURL = new RegEx("^(?:([\\w]+)\\:\\/\\/){0,1}([\\w\\.]+)(?:\\/{0,1}([\\w\\.\\-\\_])*)$", 0);
	} catch (const char *exp) {
		printf("FeedParser::ParseURL(%s): RegEx compilation failed: \"%s\"\n", url, exp);
		return status;
	};
	
	if (regURL->Search(url, PCRE_ANCHORED) == true) {
		printf("Found match!\n");
		for (int32 i = 0; i < regURL->SubStrings(); i++) {
			printf("Sub %i: \"%s\"\n", i, regURL->Match(i));
		};
	} else {
		printf("Regex failed :~(\n");
	};
	
	delete regURL;
	return status;
};
