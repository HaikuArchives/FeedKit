#include "ItemLogger.h"

#include <Directory.h>
#include <FindDirectory.h>
#include <Path.h>
#include <Volume.h>
#include <VolumeRoster.h>

#include <libfeedkit/FeedKitConstants.h>
#include <libfeedkit/FeedListener.h>
#include <libfeedkit/Feed.h>
#include <libfeedkit/Channel.h>
#include <libfeedkit/Item.h>
#include <libfeedkit/SettingsManager.h>

#include "QueryLooper.h"

//#pragma mark Namespace import

using namespace FeedKit;
using namespace FeedKit::Settings;

//#pragma mark Support Classes

class ItemDetails {
	public:
		BString channel;
		BString title;
};

//#pragma mark Constants

const char *kAppSig = "application/x-vnd.beclan.feedkit.ItemLogger";
const char *kItemPredicate = "((rss:url==*)&&(BEOS:TYPE==\"text/x-feed-item\"))";

// Message whats
const int32 kMsgQueryItem = 'iliq';

//#pragma mark Constructor

ItemLogger::ItemLogger(void)
	: BApplication("application/x-vnd.beclan.feedkit.item_logger"),
	 fFeed(NULL),
	 fSettings(NULL) {
};

ItemLogger::~ItemLogger(void) {
	fFeed->StopListening();
	BMessenger(fFeed).SendMessage(B_QUIT_REQUESTED);
	
	delete fSettings;
};

//#pragma mark BApplication Hooks
					
void ItemLogger::MessageReceived(BMessage *msg) {
	switch (msg->what) {
		case kMsgQueryItem: {
			int32 querywhat = B_ERROR;
			
			if (msg->FindInt32("query_what", &querywhat) != B_OK) return;
			
			switch (querywhat) {
				case QL::Notifications::InitialFetch: {
					for (int32 i = 0; i < fQuery->CountEntries(); i++) {
						entry_ref ref = fQuery->EntryAt(i);
						
					};
				} break;
				
				case QL::Notifications::EntryAdded: {
					entry_ref ref;
					if (msg->FindRef("affected_ref", &ref) != B_OK) return;
				} break;
				
				case QL::Notifications::EntryRemoved: {
				} break;
			};
		} break;
	
		case FeedKit::FromServer::SubscribeToComplete: {
			Feed feed;
			if (msg->FindFlat("feed", &feed) != B_OK) return;
			
			for (int32 i = 0; i < feed.ChannelCount(); i++) {
				Channel *channel = feed.ChannelAt(i);
				
				Flatten(feed.URL(), channel);
			};
		} break;
	
		case FeedKit::FromServer::NewItems: {
			printf("FeedKit::FromServer::NewItems received\n");

			const char *URL = NULL;
			if (msg->FindString("feedurl", &URL) != B_OK) return;
			
			Channel channel;
			
			for (int32 i = 0; msg->FindFlat("channel", i, &channel) == B_OK; i++) {
				Flatten(URL, &channel);
			};

		} break;
		
		case FeedKit::FromServer::SettingsUpdated: {
			fSettings->Settings().PrintToStream();
		} break;
		
		default: {
			BApplication::MessageReceived(msg);
		};
	};
};

void ItemLogger::ReadyToRun(void) {
	fFeed = new FeedListener(BMessenger(this));
	fFeed->StartListening();
	
	BVolumeRoster roster;

	volume_t volumes;
	BVolume	vol;
	
	roster.Rewind();
	
	while (roster.GetNextVolume(&vol) == B_OK) {
		if ((vol.InitCheck() != B_OK) || (vol.KnowsQuery() == false) ||
			(vol.KnowsMime() == false) || (vol.KnowsAttr() == false)) {

			continue;
		};
		
		volumes.push_back(vol);
	};

	fQuery = new QueryLooper(kItemPredicate, volumes, "Item Query",
		this, new BMessage(kMsgQueryItem));

	fSettings = new SettingsManager(AppTypes::SettingClient, "Logger");
	fSettings->SetTarget(this);
	fSettings->WatchSettings();

	BMessage settings;
	
	entry_ref homeRef;
	BPath homePath;
	find_directory(B_USER_DIRECTORY, &homePath);
	get_ref_for_path(homePath.Path(), &homeRef);
	
	BMessage path;
	path.AddInt32("type", B_REF_TYPE);
	path.AddString("name", "logpath");
	path.AddString("label", "Root path to save items to");
	path.AddString("help", "Controls where items get saved to");
	path.AddInt32("display_type", FeedKit::Settings::DirectoryPickerSingle);
	path.AddRef("default_value", &homeRef);
	
	BMessage dirName;
	dirName.AddInt32("type", B_STRING_TYPE);
	dirName.AddString("name", "feedname");
	dirName.AddString("label", "Feed directory name");
	dirName.AddString("help", "Controls the name of Feed directories. The following variables "
		"can be used;\n"
		"$URL$ - The Feed's URL\n");
	dirName.AddInt32("display_type", FeedKit::Settings::TextMulti);
	dirName.AddString("default_value", "$URL$");
	
	BMessage chanName;
	chanName.AddInt32("type", B_STRING_TYPE);
	chanName.AddString("name", "channelname");
	chanName.AddString("label", "Channel directory name");
	chanName.AddString("help", "Controls the name of the Channel directories. The following "
		"variables can be used;\n"
		"$Title$ - Title\n"
		"$Link$ - URL for the Channel\n"
		"$Description$ - Description\n"
		"$Webmaster$ - Webmaster\n"
		"$Editor$ - Editor");
	chanName.AddInt32("display_type", FeedKit::Settings::TextMulti);
	chanName.AddString("default_value", "$Title$");
	
	BMessage itemName;
	itemName.AddInt32("type", B_STRING_TYPE);
	itemName.AddString("name", "itemname");
	itemName.AddString("label", "Item filename");
	itemName.AddString("help", "Controls the name of the Item. The follow variables can be "
		"used;\n"
		"$GUID$ - Globally Unique Identifier\n"
		"$Author$ - Author\n"
		"$Title$ - Title\n"
		"$Link$ - Link\n"
		"$Category$ - Category");
	itemName.AddInt32("display_type", FeedKit::Settings::TextMulti);
	itemName.AddString("default_value", "$Author$_$Title$_$GUID$");
	
	settings.AddMessage("setting", &path);
	settings.AddMessage("setting", &dirName);
	settings.AddMessage("setting", &chanName);
	settings.AddMessage("setting", &itemName);
		
	fSettings->Template(&settings, kAppSig);
};

//#pragma mark Private

void ItemLogger::Flatten(const char *feedurl, Channel *channel) {
	BMessage settings = fSettings->Settings();

	entry_ref ref;
	if (settings.FindRef("logpath", &ref) != B_OK) return;		

	BPath logPath(&ref);
	BString temp;
	
	settings.FindString("feedname", &temp);
	
	BString feedPath(logPath.Path());
	feedPath << "/" << temp;
	feedPath.IReplaceAll("$URL$", MakeURLSafe(feedurl).String());

	BDirectory dir;
	dir.CreateDirectory(feedPath.String(), NULL);
	
	BString channelPath(feedPath);
	settings.FindString("channelname", &temp);
	channelPath << "/" << temp;
	
	channelPath.IReplaceAll("$Title$", channel->Title());
	channelPath.IReplaceAll("$Link$", MakeURLSafe(channel->Link()).String());
	channelPath.IReplaceAll("$Description$", channel->Description());
	channelPath.IReplaceAll("$Webmaster$", channel->Webmaster());
	channelPath.IReplaceAll("$Editor$", channel->Editor());						
		
	dir.CreateDirectory(channelPath.String(), NULL);
		
	for (int32 i = 0; i < channel->ItemCount(); i++) {
		Item *item = channel->ItemAt(i);
		
		BString itemPath(channelPath);
		settings.FindString("itemname", &temp);
		itemPath << "/" << temp;
		
		itemPath.IReplaceAll("$GUID$", MakeURLSafe(item->GUID()).String());
		itemPath.IReplaceAll("$Author$", item->Author());
		itemPath.IReplaceAll("$Title$", item->Title());
		itemPath.IReplaceAll("$Link$", MakeURLSafe(item->Link()).String());
		itemPath.IReplaceAll("$Category$", item->Category());
		itemPath.IReplaceAll("$", "");
		
		SaveItem(itemPath.String(), channel, item);
	};
};

status_t ItemLogger::SaveItem(const char *path, Channel *channel, Item *item) {
	printf("ItemLogger::SaveItem(%s, %s [%p], %s [%p]) called\n", path, channel->Title(), channel,
		item->Title(), item);
	
	BFile file(path, B_READ_WRITE | B_CREATE_FILE);
	file.SetSize(0);

	// File type
	file.WriteAttr("BEOS:TYPE", B_STRING_TYPE, 0, "text/x-feed-item", strlen("text/x-feed-item") + 1);
		
	// Channel info
	file.WriteAttr("feed:feedtitle", B_STRING_TYPE, 0, channel->Title(), strlen(channel->Title()));

	// Item specific
	file.WriteAttr("feed:guid", B_STRING_TYPE, 0, item->GUID(), strlen(item->GUID()));

	bool permaGUID = item->IsGUIDPermaLink();
	file.WriteAttr("feed:isGuidPermanent", B_BOOL_TYPE, 0, (const void *)&permaGUID, sizeof(permaGUID));

	file.WriteAttr("feed:author", B_STRING_TYPE, 0, item->Author(), strlen(item->Author()));
	file.WriteAttr("feed:title", B_STRING_TYPE, 0, item->Title(), strlen(item->Title()));
	file.WriteAttr("feed:link", B_STRING_TYPE, 0, item->Link(), strlen(item->Link()));
	file.WriteAttr("feed:category", B_STRING_TYPE, 0, item->Category(), strlen(item->Category()));
	file.WriteAttr("feed:comments", B_STRING_TYPE, 0, item->Comments(), strlen(item->Comments()));

	time_t date = item->Date();
	file.WriteAttr("feed:time", B_TIME_TYPE, 0, (const void *)&date, sizeof(date));

	file.WriteAttr("feed:sourceurl", B_STRING_TYPE, 0, item->SourceURL(), strlen(item->SourceURL()));
		
	file.WriteAt(0, item->Description(), strlen(item->Description()));

	int32 flatSize = item->FlattenedSize();
	char *buffer = (char *)calloc(flatSize, sizeof(char));
	
	if (item->Flatten(buffer, flatSize) == flatSize) {
		file.WriteAttr("feed:flatitem", item->TypeCode(), 0, buffer, flatSize);
	};
	free(buffer);
	
	return B_OK;

};

BString ItemLogger::MakeURLSafe(const char *url) {
	static BString newURL = "";
	if (url) {
		newURL.SetTo(url);
		newURL.ReplaceAll("/", "_");
	};
	
	return newURL;
};
