#include "FeedInfoPopper.h"

#include <Entry.h>
#include <Mime.h>
#include <Roster.h>

#include <libim/InfoPopper.h>

#include <libfeedkit/Feed.h>
#include <libfeedkit/FeedKitConstants.h>
#include <libfeedkit/FeedListener.h>
#include <libfeedkit/Channel.h>
#include <libfeedkit/Content.h>
#include <libfeedkit/ContentSpecification.h>
#include <libfeedkit/DownloadProgress.h>
#include <libfeedkit/Item.h>
#include <libfeedkit/ItemSpecification.h>
#include <libfeedkit/SettingsManager.h>
#include <libfeedkit/FeedHandler.h>

//#pragma mark Constants

const char *kAppSignature = "application/x-vnd.beclan.feedkit.InfoPopper";

//#pragma mark Constructor

FeedInfoPopper::FeedInfoPopper(void)
	: BApplication(kAppSignature), 
	 fFeedListener(NULL) {

	app_info info;
	be_roster->GetAppInfo(FeedKit::ServerSignature, &info);
	fServerRef = info.ref;
};

FeedInfoPopper::~FeedInfoPopper(void) {
	fFeedListener->StopListening();
	BMessenger(fFeedListener).SendMessage(B_QUIT_REQUESTED);
	
	delete fInfoPopper;
};

//#pragma mark BApplication Hooks
					
void FeedInfoPopper::ReadyToRun(void) {
	fFeedListener = new FeedListener();
	fFeedListener->StartListening();
	
	fFeedListener->AddHandler(this);
	
	fInfoPopper = new BMessenger(InfoPopperAppSig);
	
	FeedKit::Settings::SettingsManager manager(FeedKit::Settings::AppTypes::SettingClient, "Info Popper");

	BMessage settings;
	manager.Template(&settings, kAppSignature);
};

//#pragma mark Feed Handler Hooks

void FeedInfoPopper::FeedRegistered(Feed *feed) {
	fprintf(stderr, "FeedInfoPopper::FeedRegistered([%p])\n", feed);

	for (int32 i = 0; i < feed->ChannelCount(); i++) {
		Channel *channel = feed->ChannelAt(i);
		
		BroadcastChannelContents(channel);
	};
};

void FeedInfoPopper::ChannelUpdated(Feed *feed, Channel *channel) {
	fprintf(stderr, "FeedInfoPopper::ChannelUpdated([%p], [%p])\n", feed, channel);
	
	BroadcastChannelContents(channel);
};

void FeedInfoPopper::EnclosureDownloadStarted(Feed */*feed */, Item */*item*/, Enclosure */*enclosure*/, entry_ref /*ref*/) {
};

void FeedInfoPopper::EnclosureDownloadProgress(Feed *feed, Item *item, Enclosure *enclosure, DownloadProgress *progress, entry_ref /*ref*/) {
	BroadcastDownloadProgress(feed, item, enclosure, progress);
};

//void FeedInfoPopper::EnclosureDownloadFinished(Feed *feed, Item *item, Enclosure *enclosure, entry_ref ref) {
//};
//
//void FeedInfoPopper::EnclosureDownloadError(Feed *feed, Item *item, Enclosure *enclosure, Error *error) {
//};

//#pragma mark Private

void FeedInfoPopper::BroadcastChannelContents(Channel *channel) {
	bool useChannelIcon = false;
	entry_ref channelIcon;

	fprintf(stderr, "FeedInfoPopper::BroadcastChannelContents([%p] => %s)\n", channel, channel->Title());

	// Get the Channel Icon, if one exists
	if (channel->Image() != NULL) {
		const char *iconPath = fFeedListener->ChannelIconPath(channel);
		if (iconPath != NULL) {
			get_ref_for_path(iconPath, &channelIcon);
			
			useChannelIcon = true;
		};
	};

	ItemList newItems = channel->FindItems(new NewItemSpecification(), true);
	for (ItemList::iterator nIt = newItems.begin(); nIt != newItems.end(); nIt++) {
		Item *item = *nIt;
		if (item == NULL) continue;

		BMessage info(InfoPopper::AddMessage);
		info.AddInt8("type", InfoPopper::Information);
		info.AddString("app", "The Feed Kit");
		info.AddString("title", "New Item");
		
		BString content = channel->Title();
		content << " - " << item->Title() << "\n";
		
		ContentList itemContents;
		itemContents = item->FindContent(new MIMEContentSpecification("text/plain"), true);
		if (itemContents.empty() == true) {
			itemContents = item->FindContent(new MIMESuperTypeContentSpecification("text/"), true);
			
			if (itemContents.empty() == true) {
				itemContents = item->FindContent(new AllContentSpecification(), true);
			};
		};
		
		if (itemContents.empty() == false) {
			content << itemContents[0]->Text();
		};
						
		info.AddString("content", content);
		info.AddString("messageID", item->GUID());
		
		if (useChannelIcon == false) {
			info.AddRef("iconRef", &fServerRef);
			info.AddInt32("iconType", InfoPopper::Attribute);
		} else {
			info.AddRef("iconRef", &channelIcon);
			info.AddInt32("iconType", InfoPopper::Contents);
			
			info.AddRef("overlayIconRef", &fServerRef);
			info.AddInt32("overlayIconType", InfoPopper::Attribute);
		};

		fInfoPopper->SendMessage(&info);
	};
};

void FeedInfoPopper::BroadcastDownloadProgress(Feed */*feed*/, Item */*item*/, Enclosure *enclosure, DownloadProgress *progress) {
	BMessage info(InfoPopper::AddMessage);
	info.AddInt8("type", InfoPopper::Progress);
	info.AddString("app", "The Feed Kit");
	info.AddString("title", "Downloading...");
	
	char start[1024];
	char end[1024];
	char eta[1024];
	char buffer[2048];
	
	time_t tempTime = progress->StartTime();			
	strftime(start, sizeof(start), "%H:%M:%S", localtime(&tempTime));
	tempTime = progress->EndTime();
	strftime(end, sizeof(end), "%H:%M:%S", localtime(&tempTime));
	
	float size = (float)(progress->Size() / 1024.0f);
	float downloaded = ((float)progress->AmountDownloaded() / 1024.0f);
	
	char shortDesc[B_MIME_TYPE_LENGTH];
	BMimeType type(progress->MIME());
	type.GetShortDescription(shortDesc);
					
	sprintf(buffer, "URL: %s\nType: %s (%s)\nDescription: %s\n"
		"Time: %s - %s\nSize: %.1f kb of %.1f kb (%.1f kb/s)",
		progress->URL(), shortDesc, progress->MIME(), progress->Description(), start, end,
		downloaded, size, progress->Speed() / 1024.0f); 

	info.AddString("content", buffer);
	info.AddString("messageID", enclosure->UUID());
	info.AddFloat("progress", progress->PercentageComplete());
	
	info.AddRef("iconRef", &fServerRef);
	info.AddInt32("iconType", InfoPopper::Attribute);
	
	fInfoPopper->SendMessage(&info);
};