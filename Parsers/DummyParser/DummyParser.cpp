#include "DummyParser.h"

#include <stdio.h>

//#pragma mark C Hooks

extern "C" {
	FeedParser *instantiate_parser(void) {
		return new DummyParser();
	};
};

//#pragma mark Constants

//#pragma mark Constructor

DummyParser::DummyParser(void) {
	printf("New!\n");
};

DummyParser::~DummyParser(void) {
	printf("~DummyParser() called\n");
};

//#pragma mark FeedParser Hooks

float DummyParser::HandlesFeedType(const char *feedtype) {
	return 1.0f;
};

float DummyParser::HandlesFeed(const char *feed, int32 length) {
	return 1.0f;
};

Channel *DummyParser::ParseFeed(const char *feed, int32 length) {
	return NULL;
};

const char *DummyParser::Name(void) {
	return "DummyParser";
};
