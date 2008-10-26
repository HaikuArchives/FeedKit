#include "FeedInfoPopper.h"

#include <Entry.h>
#include <Mime.h>
#include <Roster.h>

#include <libim/InfoPopper.h>

#include <libfeedkit/Feed.h>
#include <libfeedkit/FeedKitConstants.h>
#include <libfeedkit/FeedListener.h>
#include <libfeedkit/Channel.h>
#include <libfeedkit/DownloadProgress.h>
#include <libfeedkit/Item.h>
#include <libfeedkit/SettingsManager.h>

#include <libfeedkit/FeedHandler.h>

//#pragma mark Constants

const char *kAppSignature = "application/x-vnd.beclan.feedkit.InfoPopper";

//#pragma mark Constructor

FeedInfoPopper::FeedInfoPopper(void)
	: BApplication("application/x-vnd.beclan.feedkit.InfoPopperClient"),
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
					
void FeedInfoPopper::MessageReceived(BMessage *msg) {
#if 0
	switch (msg->what) {
		default: {
			BApplication::MessageReceived(msg);
		} break;
	};
#endif
};

void FeedInfoPopper::ReadyToRun(void) {
	fFeedListener = new FeedListener();//BMessenger(this));
	fFeedListener->StartListening();
	
	fFeedListener->AddHandler(this);
	
	fInfoPopper = new BMessenger(InfoPopperAppSig);
	
	FeedKit::Settings::SettingsManager manager(FeedKit::Settings::AppTypes::SettingClient, "Info Popper");

	BMessage settings;

	status_t result = manager.Template(&settings, kAppSignature);
};

//#pragma mark Feed Handler Hooks

void FeedInfoPopper::FeedSubscribed(Feed *feed) {
	fprintf(stderr, "FeedInfoPopper::FeedSubscribed([%p])\n", feed);

	for (int32 i = 0; i < feed->ChannelCount(); i++) {
		Channel *channel = feed->ChannelAt(i);
		
		BroadcastChannelContents(channel);
	};
};

void FeedInfoPopper::ChannelUpdated(Feed *feed, Channel *channel) {
	fprintf(stderr, "FeedInfoPopper::ChannelUpdated([%p], [%p])\n", feed, channel);
	
	BroadcastChannelContents(channel);
};

//#pragma mark Private

void FeedInfoPopper::BroadcastChannelContents(Channel *channel) {
printf("FeedInfoPopper::BroadcastChannelContents([%p] => %s)\n", channel, channel->Title());
	bool useChannelIcon = false;
	entry_ref channelIcon;

printf("Image? %p\n", channel->Image());

	// Get the Channel Icon, if one exists
	if (channel->Image() != NULL) {
		const char *iconPath = fFeedListener->ChannelIconPath(channel);
		if (iconPath != NULL) {
			get_ref_for_path(iconPath, &channelIcon);
			
			useChannelIcon = true;
		};
	};

	for (int32 i = 0; i < channel->NewItemCount(); i++) {
		Item *item = channel->NewItemAt(i);
		if (item == NULL) continue;

		BMessage info(InfoPopper::AddMessage);
		info.AddInt8("type", InfoPopper::Information);
		info.AddString("app", "The Feed Kit");
		info.AddString("title", "New Item");
		
		BString content = channel->Title();
		content << " - " << item->Title() << "\n";
		content << item->Description();
						
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


