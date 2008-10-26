#include "FeedHandler.h"

#include "Feed.h"
#include "Channel.h"
#include "Item.h"

#include <stdio.h>

#include <Entry.h>

using namespace FeedKit;

//#pragma mark Constructor

FeedHandler::FeedHandler(void) {
};

FeedHandler::~FeedHandler(void) {
}
		
//#pragma mark Overridable Hooks
void FeedHandler::ServerStarted(void) {
};

void FeedHandler::ServerShutdown(void) {
};

void FeedHandler::FeedRegistered(Feed *feed) {
};

void FeedHandler::FeedRegisteredError(Feed *feed, ErrorDetails *error) {
};

void FeedHandler::FeedSubscriptionChanged(Feed *feed) {
};

void FeedHandler::ChannelUpdated(Feed *feed, Channel *channel) {
};

void FeedHandler::ChannelIconUpdated(Feed *feed, Channel *channel, entry_ref ref) {
};

void FeedHandler::ItemRead(Feed *feed, Channel *channel, Item *item) {
};

void FeedHandler::EnclosureDownloadStarted(Feed *feed, Item *item, Enclosure *enclosure) {
};

void FeedHandler::EnclosureDownloadProgress(Feed *feed, Item *item, Enclosure *enclosure) {
};

void FeedHandler::EnclosureDownloadFinished(Feed *feed, Item *item, Enclosure *enclosure) {
};

void FeedHandler::EnclosureDownloadError(Feed *feed, Item *item, Enclosure *enclosure, ErrorDetails *error) {
};

void FeedHandler::EnclosureDownloadStatusChanged(Feed *feed, Item *item, Enclosure *enclosure) {
};

void FeedHandler::FeedDownloadStarted(Feed *feed, DownloadProgress *progress) {
};

void FeedHandler::FeedDownloadProgress(Feed *feed, DownloadProgress *progress) {
};

void FeedHandler::FeedDownloadFinished(Feed *feed, DownloadProgress *progress) {
};

void FeedHandler::FeedDownloadError(Feed *feed, DownloadProgress *progress) {
};
