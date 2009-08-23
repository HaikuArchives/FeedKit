#include "TraceFeedHandler.h"

#include "Feed.h"
#include "Channel.h"
#include "Item.h"

#include <stdio.h>

using namespace FeedKit;

//#pragma mark Constructor

TraceFeedHandler::TraceFeedHandler(FILE *output)
	: FeedHandler(),
	fOutput(output) {
};

TraceFeedHandler::~TraceFeedHandler(void) {
};
		
//#pragma mark Overridable Hooks
void TraceFeedHandler::ServerStarted(void) {
	fprintf(fOutput, "TraceFeedHandler::ServerStarted()\n");
};

void TraceFeedHandler::ServerShutdown(void) {
	fprintf(fOutput, "TraceFeedHandler::ServerShutdown()\n");
};

void TraceFeedHandler::FeedRegistered(Feed *feed) {
	fprintf(fOutput, "TraceFeedHandler::FeedRegistered([%p])\n", feed);
};

void TraceFeedHandler::FeedRegisteredError(Feed *feed, ErrorDetails *error) {
	fprintf(fOutput, "TraceFeedHandler::FeedRegisteredError([%p], [%p])\n", feed, error);
};

void TraceFeedHandler::FeedSubscriptionChanged(Feed *feed) {
	fprintf(fOutput, "TraceFeedHandler::FeedSubscriptionChanged([%p]) - %s is now %ssubscribed\n", feed, feed->URL(), feed->Subscribed() ? "" : "un");
};

void TraceFeedHandler::ChannelUpdated(Feed *feed, Channel *channel) {
	fprintf(fOutput, "TraceFeedHandler::ChannelUpdated([%p], [%p])\n", feed, channel);
};

void TraceFeedHandler::ChannelIconUpdated(Feed *feed, Channel *channel, entry_ref ref) {
	fprintf(fOutput, "TraceFeedHandler::ChannelIconUpdated([%p], [%p], %s)\n", feed, channel, ref.name);
};

void TraceFeedHandler::ItemRead(Feed *feed, Channel *channel, Item *item) {
	fprintf(fOutput, "TraceFeedHandler::ItemRead([%p], [%p], [%p])\n", feed, channel, item);
};

void TraceFeedHandler::EnclosureDownloadStarted(Feed *feed, Item *item, Enclosure *enclosure) {
	fprintf(fOutput, "TraceFeedHandler::EnclosureDownloadStarted([%p], [%p], [%p])\n", feed, item, enclosure);
};

void TraceFeedHandler::EnclosureDownloadProgress(Feed *feed, Item *item, Enclosure *enclosure) {
	fprintf(fOutput, "TraceFeedHandler::EnclosureDownloadProgress([%p], [%p], [%p])\n", feed, item, enclosure);
};

void TraceFeedHandler::EnclosureDownloadFinished(Feed *feed, Item *item, Enclosure *enclosure) {
	fprintf(fOutput, "TraceFeedHandler::EnclosureDownloadFinished([%p], [%p], [%p])\n", feed, item, enclosure);
};

void TraceFeedHandler::EnclosureDownloadError(Feed *feed, Item *item, Enclosure *enclosure, ErrorDetails *error) {
	fprintf(fOutput, "TraceFeedHandler::EnclosureDownloadError([%p], [%p], [%p], [%p], %s)\n", feed, item, enclosure, error);
};

void TraceFeedHandler::EnclosureDownloadStatusChanged(Feed *feed, Item *item, Enclosure *enclosure) {
	fprintf(fOutput, "TraceFeedHandler::EnclosureDownloadStatusChanged([%p], [%p], [%p])\n", feed, item, enclosure);
};


void TraceFeedHandler::FeedDownloadStarted(Feed *feed, DownloadProgress *progress) {
	fprintf(fOutput, "TraceFeedHandler::FeedDownloadStarted([%p], [%p])\n", feed, progress);
};

void TraceFeedHandler::FeedDownloadProgress(Feed *feed, DownloadProgress *progress) {
	fprintf(fOutput, "TraceFeedHandler::FeedDownloadProgress([%p], [%p])\n", feed, progress);
};

void TraceFeedHandler::FeedDownloadFinished(Feed *feed, DownloadProgress *progress) {
	fprintf(fOutput, "TraceFeedHandler::FeedDownloadFinished([%p], [%p])\n", feed, progress);
};

void TraceFeedHandler::FeedDownloadError(Feed *feed, DownloadProgress *progress) {
	fprintf(fOutput, "TraceFeedHandler::FeedDownloadError([%p], [%p])\n", feed, progress);
};
