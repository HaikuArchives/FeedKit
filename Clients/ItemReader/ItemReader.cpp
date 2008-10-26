#include "ItemReader.h"

#include <Directory.h>
#include <FindDirectory.h>
#include <Path.h>
#include <Roster.h>
#include <Volume.h>
#include <VolumeRoster.h>

#include <libfeedkit/FeedKitConstants.h>
#include <libfeedkit/FeedListener.h>
#include <libfeedkit/Feed.h>
#include <libfeedkit/Channel.h>
#include <libfeedkit/Item.h>
#include <libfeedkit/SettingsManager.h>

#include "IMKitUtilities.h"

//#pragma mark Namespace import

using namespace FeedKit;

//#pragma mark Support Classes

//#pragma mark Constants

//#pragma mark Constructor

ItemReader::ItemReader(void)
	: BApplication("application/x-vnd.beclan.feedkit.ItemReader"),
	 fFeed(NULL) {
};

ItemReader::~ItemReader(void) {
	fFeed->StopListening();
	BMessenger(fFeed).SendMessage(B_QUIT_REQUESTED);
};

//#pragma mark BApplication Hooks

void ItemReader::ReadyToRun(void) {
	fFeed = new FeedListener(BMessenger(this));
	fFeed->StartListening();
};

void ItemReader::RefsReceived(BMessage *msg) {
	entry_ref ref;
	for (int32 i = 0; msg->FindRef("refs", i, &ref) == B_OK; i++) {
		MarkAsRead(ref);
		LaunchItem(ref);
	};
};

//#pragma mark Private

void ItemReader::LaunchItem(entry_ref ref) {
	bool foundURL = false;
	
	entry_ref htmlRef;
	be_roster->FindApp("application/x-vnd.Be.URL.http", &htmlRef);
	BPath htmlPath(&htmlRef);

	BMessage argv(B_ARGV_RECEIVED);
	argv.AddString("argv", htmlPath.Path());
	
	BPath path(&ref);
	int32 length = -1;
	char *url = ReadAttribute(path.Path(), "META:url", &length);
	if ((url != NULL) && (length > 1)) {
		url = (char *)realloc(url, (length + 1) * sizeof(char));
		url[length] = '\0';
		argv.AddString("argv", url);
		
		foundURL = true;
	};
	if (url) free(url);
	
	if (foundURL > 0) {
		argv.AddInt32("argc", 2);
		be_roster->Launch(&htmlRef, &argv);
	};
};

void ItemReader::MarkAsRead(entry_ref ref) {
	int32 length = B_ERROR;
	char *data = ReadAttribute(BNode(&ref), "feed:flatitem", &length);
	if (length > 0) {
		Item item;
		if (item.Unflatten(item.TypeCode(), data, length) == B_OK) {
			BMessage markRead(FeedKit::ToServer::MarkRead);
			markRead.AddFlat("item", &item);
			
			fFeed->SendMessage(&markRead);
		};
	};
	
	free(data);
};
