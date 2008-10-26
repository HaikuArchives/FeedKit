#include "EnclosureDownloader.h"

#include <libfeedkit/FeedKitConstants.h>
#include <libfeedkit/FeedListener.h>
#include <libfeedkit/Feed.h>
#include <libfeedkit/Channel.h>
#include <libfeedkit/Item.h>
#include <libfeedkit/ItemSpecification.h>
#include <libfeedkit/EnclosureSpecification.h>
#include <libfeedkit/SettingsManager.h>

#include <FindDirectory.h>

//#pragma mark DownloadedEnclosureSpecification

class DownloadedEnclosureSpecification : public EnclosureSpecification {
	virtual bool 	IsSatisfiedBy(Enclosure *enc) {
//						return enc->Downloaded();
return false;
					};
};

//#pragma mark Namespace import

using namespace FeedKit;
using namespace FeedKit::Settings;

//#pragma mark Constants

//#Message whats

//#pragma mark Constructor

EnclosureDownloader::EnclosureDownloader(void)
	: BApplication("application/x-vnd.beclan.feedkit.EnclosureDownloader"),
	 fFeed(NULL),
	 fSettingsMan(NULL) {
};

EnclosureDownloader::~EnclosureDownloader(void) {
	BMessenger(fFeed).SendMessage(B_QUIT_REQUESTED);
	fFeed->StopListening();
	
	delete fSettingsMan;
};

//#pragma mark BApplication Hooks

void EnclosureDownloader::ReadyToRun(void) {
	fFeed = new FeedListener();
	fFeed->StartListening();
	fFeed->AddHandler(this);
	
	fSettingsMan = new SettingsManager(AppTypes::SettingClient, "Enclosure Downloader");
	fSettingsMan->SetTarget(this);

	BMessage settings;
	
	entry_ref homeRef;
	BPath homePath;
	find_directory(B_USER_DIRECTORY, &homePath);
	get_ref_for_path(homePath.Path(), &homeRef);
	
	BMessage path;
	path.AddInt32("type", B_REF_TYPE);
	path.AddString("name", "enclosurepath");
	path.AddString("label", "Root path to save items to");
	path.AddString("help", "Controls where items get saved to");
	path.AddInt32("display_type", FeedKit::Settings::DirectoryPickerSingle);
	path.AddRef("default_value", &homeRef);

	settings.AddMessage("setting", &path);
		
	fSettingsMan->Template(&settings);
	fSettingsMan->WatchSettings();
	
	// Force a read of the settings
	BMessenger(this).SendMessage(FeedKit::FromServer::SettingsUpdated);
};

void EnclosureDownloader::MessageReceived(BMessage *msg) {
	switch (msg->what) {
		case B_NODE_MONITOR:
		case FeedKit::FromServer::SettingsUpdated: {
			BMessage settings = fSettingsMan->Settings();
			if (settings.FindRef("enclosurepath", &fEnclosurePath) != B_OK) {
				BPath homePath;
				find_directory(B_USER_DIRECTORY, &homePath);
				get_ref_for_path(homePath.Path(), &fEnclosurePath);
			};
		} break;
		
		default: {
			BApplication::MessageReceived(msg);
		};
	};
};

//#pragma mark FeedHandler Hooks

void EnclosureDownloader::FeedRegistered(Feed *feed) {
	fprintf(stderr, "EnclosureDownloader::FeedSubscribed([%p])\n", feed);

	for (int32 i = 0; i < feed->ChannelCount(); i++) {
		Channel *channel = feed->ChannelAt(i);
	
		for (int32 j = 0; j < channel->ItemCount(); j++) {
			DownloadItemEnclosures(channel->ItemAt(j));
		};
	};
};

void EnclosureDownloader::ChannelUpdated(Feed *feed, Channel *channel) {
	fprintf(stderr, "EnclosureDownloader::ChannelUpdated([%p], [%p])\n", feed, channel);
	ItemList newItems = channel->FindItems(new NewItemSpecification(), true);

	for (ItemList::iterator iIt = newItems.begin(); iIt != newItems.end(); iIt++) {
		DownloadItemEnclosures(*iIt);
	};
};

//#pragma mark Private

void EnclosureDownloader::DownloadItemEnclosures(Item *item) {
	// Find items that haven't been downloaded
	EnclosureSpecification *spec = new NotSpecification<Enclosure *>(new DownloadedEnclosureSpecification());
	EnclosureList enclosures = item->FindEnclosures(spec, true);

	fprintf(stderr, "EnclosureDownloader::DownloadItemEnclosures([%p]) - will download %i of %i enclosures\n", item, enclosures.size(), item->EnclosureCount());
		
	for (EnclosureList::iterator eIt = enclosures.begin(); eIt != enclosures.end(); eIt++) {
		Enclosure *enclosure = *eIt;
		
		BPath path(&fEnclosurePath);
		BString url = enclosure->URL();
		int32 slash = url.IFindLast("/");

		if (slash != B_ERROR) {
			url.Remove(0, slash + 1);
		};
		printf("URL: %s / %i / %s\n", enclosure->URL(), slash, url.String());
		
		path.Append(url.String());
		printf("Path: %s\n", path.Path());
//		path.Append(enclosure->UUID());
		
		fFeed->DownloadEnclosure(item, enclosure, path.Path());
	};
}