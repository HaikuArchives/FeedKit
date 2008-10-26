#include "FeedIcon.h"
#include "Constants.h"

#include "FeedRegistrationWindow.h"

#include <Bitmap.h>
#include <Deskbar.h>
#include <FindDirectory.h>
#include <Menu.h>
#include <MenuItem.h>
#include <NodeInfo.h>
#include <Roster.h>
#include <PopUpMenu.h>
#include <Window.h>

#ifdef B_ZETA_VERSION
#include <sys_apps/Tracker/Icons.h>
#endif

#include <libfeedkit/Enclosure.h>
#include <libfeedkit/Feed.h>
#include <libfeedkit/FeedKitConstants.h>
#include <libfeedkit/FeedListener.h>
#include <libfeedkit/Channel.h>
#include <libfeedkit/Item.h>
#include <libfeedkit/ItemSpecification.h>
#include <libfeedkit/SettingsManager.h>
#include <libfeedkit/Specification.h>

#include <stdio.h>

//#pragma mark Namespace import

using namespace FeedKit;

//#pragma mark AllItemSpecificatin

class AllItemSpecification : public ItemSpecification {
	virtual bool IsSatisfiedBy(Item */*item*/) {
		return true;
	};
};

//#pragma mark Extern C

extern "C" {
	BView *instantiate_deskbar_item(void) {
		return new FeedIcon();
	};
};

//#pragma mark Constants

const float kRectSize = 15.0f;
const float kIconSize = kRectSize + 1.0f;

const int32 kMsgQuit = 'DBIQ';
const int32 kMsgSettings = 'DBIS';
const int32 kMsgRefresh = 'DBRF';
const int32 kMsgMarkRead = 'DBMR';
const int32 kMsgRegisterFeedWin = 'DBST';
const int32 kMsgRegisterFeedConfirm = 'DBSO';
const int32 kMsgRegisterFeedCancel = 'DBSC';

const int32 kMenuOptionAll = 0;
const int32 kMenuOptionUnread = 1;
const int32 kMenuOptionCurrent = 2;
const int32 kMenuOptionUnreadCurrent = 3;

//#pragma mark Constructor

FeedIcon::FeedIcon(void)
	: BView(BRect(0, 0, kRectSize, kRectSize), VIEW_NAME, B_FOLLOW_NONE, B_WILL_DRAW),
	fFeed(NULL),
	fSettingsMan(NULL),
	fMenuSpecification(NULL) {
	
	Init();
};

FeedIcon::FeedIcon(BMessage *archive)
	: BView(archive),
	fMenuSpecification(NULL) {
	
	Init();
};

FeedIcon::~FeedIcon(void) {
	delete fNormalIcon;
	delete fNewItemsIcon;
	delete fOfflineIcon;
	delete fFeedMenu;
	
	delete fSettingsMan;
};

//#pragma mark BArchivable Hooks

status_t FeedIcon::Archive(BMessage *archive, bool deep = true) const {
	status_t result = BView::Archive(archive, deep);
	
	archive->AddString("add_on", APP_SIG);
	archive->AddString("add-on", APP_SIG);
	archive->AddString("Class", ICON_NAME);
	
	return result;
};

BArchivable *FeedIcon::Instantiate(BMessage *archive) {
	BArchivable *result = NULL;
	if (validate_instantiation(archive, ICON_NAME) == true) result = new FeedIcon(archive);
	
	return result;
};

//#pragma mark BView Hooks

void FeedIcon::Draw(BRect rect) {
	(void)rect;

	SetHighColor(Parent()->ViewColor());
	FillRect(Bounds());

	if (fIcon) {
		SetDrawingMode(B_OP_ALPHA);	
		DrawBitmap(fIcon, BPoint(0, 0));
	};
};

void FeedIcon::MouseDown(BPoint point) {
	int32 buttons;
	Window()->CurrentMessage()->FindInt32("buttons", &buttons);
	
	if (buttons & B_SECONDARY_MOUSE_BUTTON) {
		fMenu = new BPopUpMenu("FeedIcon_Menu", false, false);
		fMenu->SetFont(be_plain_font);
		
		BMenu *feed = FeedMenu();
		if (feed != NULL) {
			feed->SetTargetForItems(this);
			fMenu->AddItem(feed);
			fMenu->AddSeparatorItem();
			feed->SetTargetForItems(this);
		};
		fMenu->AddItem(new BMenuItem("Settings", new BMessage(kMsgSettings)));
		fMenu->AddItem(new BMenuItem("Quit", new BMessage(kMsgQuit)));
		
		ConvertToScreen(&point);
		BRect r(point, point);
		r.InsetBySelf(-2, -2);
		
		fMenu->SetTargetForItems(this);
		fMenu->Go(point, true, true, r, true);
		// XXX
//		fMenu->SetAsyncAutoDestruct(true);
	};
};

void FeedIcon::AttachedToWindow(void) {
	fFeed = new FeedListener();
	fFeed->StartListening();
	fFeed->AddHandler(this);

	fSettingsMan = new SettingsManager(AppTypes::SettingClient, "Deskbar Icon");
	fSettingsMan->SetTarget(this);
	
	BMessage settings;
	
	BMessage display;
	display.AddInt32("type", B_INT32_TYPE);
	display.AddString("name", "displayitems");
	display.AddString("label", "Items to display");
	display.AddString("help", "Controls what items will be displayed for feeds");
	display.AddInt32("display_type", FeedKit::Settings::MenuSingle);
	display.AddInt32("default_value", 0);
	display.AddInt32("option_value", 0);
	display.AddString("option_label", "All items");
	display.AddInt32("option_value", 1);
	display.AddString("option_label", "Unread items");
	display.AddInt32("option_value", 2);
	display.AddString("option_label", "Current items");
	display.AddInt32("option_value", 3);
	display.AddString("option_label", "Unread current items");
	
	settings.AddMessage("setting", &display);
		
	status_t result = fSettingsMan->Template(&settings, APP_SIG);
	fprintf(stderr, "FeedIcon::AttachedToWindow(): Fetch settings: %s\n", strerror(result));

	fSettingsMan->WatchSettings();
	fSettings = fSettingsMan->Settings();

	HandleSettings(&fSettings);	
//	BMessenger(this).SendMessage(FeedKit::FromServer::SettingsUpdated);

	if (fFeed->ServerIsRunning() == true) fIcon = fNormalIcon;
	if (HasUnreadItems() == true) fIcon = fNewItemsIcon;

	Invalidate();
};

void FeedIcon::DetachedFromWindow(void) {
	fFeed->StopListening();
	BMessenger(fFeed).SendMessage(B_QUIT_REQUESTED);
};
					
void FeedIcon::MessageReceived(BMessage *msg) {
	switch (msg->what) {
		case kMsgSettings: {
			be_roster->Launch("application/x-vnd.beclan-FeedKit-Prefs");
		} break;
		
		case kMsgQuit: {
			fFeed->SendMessage(new BMessage(B_QUIT_REQUESTED));
		} break;
		
		case kMsgRefresh: {
			Feed feed;
			if (msg->FindFlat("feed", &feed) == B_OK) {
				fFeed->RefreshFeed(&feed);
			} else {
				fFeed->RefreshAllFeeds();
			};
		} break;
		
		case kMsgMarkRead: {
			Item item;
			if (msg->FindFlat("item", &item) != B_OK) return;
			
			fFeed->MarkItemRead(&item);
		} break;
		
		case kMsgRegisterFeedWin: {
			FeedRegistrationWindow *sub = new FeedRegistrationWindow(new BMessenger(this),
				new BMessage(kMsgRegisterFeedCancel), new BMessage(kMsgRegisterFeedConfirm));
			
			sub->Show();
		} break;
		
		case kMsgRegisterFeedConfirm: {
			const char *url = NULL;
			const char *name = NULL;

			if (msg->FindString("url", &url) != B_OK) return;
			if (msg->FindString("name", &name) != B_OK) name = "";

			fFeed->RegisterFeed(url, name);
		} break;

		case B_NODE_MONITOR:
		case FeedKit::FromServer::SettingsUpdated: {
			fSettings = fSettingsMan->Settings();
			HandleSettings(&fSettings);
		
			fDirtyMenu = true;
			Invalidate();
		} break;

		default: {
			BView::MessageReceived(msg);
		};
	};
};

//#pragma mark Feed Handler Hooks

void FeedIcon::ServerStarted(void) {
	Looper()->Lock();

	Rebuild();

	fSettings = fSettingsMan->Settings();
	
	fDirtyMenu = true;
	Invalidate();
	
	Looper()->Unlock();
};

void FeedIcon::ServerShutdown(void) {
	Looper()->Lock();

	fIcon = fOfflineIcon;
	fDirtyMenu = true;
	Invalidate();
	
	Looper()->Unlock();
};


void FeedIcon::FeedRegistered(Feed */*feed*/) {
	Rebuild();
};

void FeedIcon::FeedSubscriptionChanged(Feed */*feed*/) {
	Rebuild();
}

void FeedIcon::ChannelUpdated(Feed */*feed*/, Channel */*channel*/) {
	Rebuild();
};

void FeedIcon::ItemRead(Feed */*feed*/, Channel */*channel*/, Item */*item*/) {
	fprintf(stderr, "FeedIcon::ItemRead()\n");

	Rebuild();
};

//#pragma mark Private

void FeedIcon::Init(void) {
	fFeed = NULL;
	fSettingsMan = NULL;
	fOfflineIcon = NULL;
	fNormalIcon = NULL;
	fNewItemsIcon = NULL;
	fIcon = NULL;
	fMenu = NULL;
	fFeedMenu = NULL;
	fDirtyMenu = true;

	entry_ref ref;
	BPath settingsPath;

	if (find_directory(B_USER_SETTINGS_DIRECTORY, &settingsPath) == B_OK) {
		settingsPath.Append("BeClan/FeedKit/Icons/");

		fOfflineIcon =  ReadNodeIcon(BString(settingsPath.Path()).Append("/server-inactive.png"), kIconSize);
		fNormalIcon =  ReadNodeIcon(BString(settingsPath.Path()).Append("/server-active.png"), kIconSize);
		fNewItemsIcon =  ReadNodeIcon(BString(settingsPath.Path()).Append("/newitems.png"), kIconSize);

		fIcon = fOfflineIcon;
	};
};

BBitmap *FeedIcon::ReadNodeIcon(BString &path, int32 size, bool followSymlink = true) {
	BBitmap *ret = NULL;
	entry_ref ref;
	
	fprintf(stderr, "FeedIcon::ReadNodeIcon(%s, %i, %s)\n", path.String(), size, followSymlink == true ? "true" : "false");
	
	if (get_ref_for_path(path.String(), &ref) == B_OK) {
	
#if defined(B_BEOS_VERSION_DANO) && (B_BEOS_VERSION > B_BEOS_VERSION_DANO)
 #ifndef B_ZETA_VERSION
		// Zeta RC2 or earlier code.
		BNode node(&ref);
		ret = GetTrackerIcon(node, size, NULL);
 #else
		// Zeta RC3 or later.
		BEntry entry(&ref, followSymlink);
		ret = new BBitmap(GetTrackerIcon(entry, size));
 #endif

#else
		// R5
		BNode node(&ref);
	
		ret = new BBitmap(BRect(0, 0, size - 1, size - 1), B_CMAP8);
		if (BNodeInfo::GetTrackerIcon((const entry_ref *)&ref, ret, (icon_size)size) < B_OK) {
			delete ret;
			ret = NULL;
		};
#endif
	}

	return ret;
};

BMenu *FeedIcon::FeedMenu(void) {
	if (fDirtyMenu) {
		delete fFeedMenu;
		fFeedMenu = NULL;
		if (fFeed->ServerIsRunning() == false) return fFeedMenu;

		fFeedMenu = new BMenu("Feeds");			
		fFeedMenu->AddItem(new BMenuItem("Refresh all feeds", new BMessage(kMsgRefresh)));
		fFeedMenu->AddItem(new BMenuItem("Register Feed" B_UTF8_ELLIPSIS, new BMessage(kMsgRegisterFeedWin)));
		fFeedMenu->AddSeparatorItem();
		
		feed_list_t feeds = fFeed->Feeds();		
		for (feed_list_t::iterator it = feeds.begin(); it != feeds.end(); it++) {
			Feed *feed = (*it);
			BMenu *feedMenu = new BMenu(feed->DisplayName());
			
			BMessage *refreshMsg = new BMessage(kMsgRefresh);
			refreshMsg->AddFlat("feed", feed);
			BMenuItem *refresh = new BMenuItem("Refresh feed", refreshMsg);
			feedMenu->AddItem(refresh);
			refresh->SetTarget(this);
			feedMenu->AddSeparatorItem();
			
			for (uint32 j = 0; j < feed->ChannelCount(); j++) {
				Channel *channel = feed->ChannelAt(j);
				BMenu *chanMenu = ChannelMenuItems(channel);

				if (chanMenu != NULL) {
					chanMenu->SetFont(be_plain_font);
					chanMenu->SetTargetForItems(this);
	
					feedMenu->AddItem(chanMenu);
				};
			};
					

			feedMenu->SetFont(be_plain_font);
			feedMenu->SetTargetForItems(this);

			fFeedMenu->AddItem(feedMenu);
		};
				
		fFeedMenu->SetFont(be_plain_font);
		fFeedMenu->SetTargetForItems(this);
		
		fDirtyMenu = false;
	};

	return fFeedMenu;
};

BMenu *FeedIcon::ChannelMenuItems(Channel *channel) {
	ItemList items = channel->FindItems(fMenuSpecification, false);
	BMenu *chanMenu = new BMenu(channel->Title());
	
	for (ItemList::iterator iIt = items.begin(); iIt != items.end(); iIt++) {
		Item *item = *iIt;
		int32 enclosures = item->EnclosureCount();
		
		BString title = item->Title();
		if (enclosures > 0) {
			title << " (" << enclosures << " enclosure";
			if (enclosures > 1) title << "s";
			title << ")";
		};

		BMessage *markReadMsg = new BMessage(kMsgMarkRead);
		markReadMsg->AddFlat("item", item);	
		
		BMenuItem *mItem = new BMenuItem(title.String(), markReadMsg);
		chanMenu->AddItem(mItem);
		mItem->SetTarget(this);
	};
	
	return chanMenu;
};

void FeedIcon::Rebuild(void) {
	Looper()->Lock();

	if (HasUnreadItems() == false) {
		fIcon = fNormalIcon;
	} else {
		fIcon = fNewItemsIcon;
	};
	
	fDirtyMenu = true;
	Invalidate();
	
	Looper()->Unlock();
};

bool FeedIcon::HasUnreadItems(void) {
	int32 unread = 0;
	feed_list_t feeds = fFeed->Feeds();
	
	for (feed_list_t::iterator it = feeds.begin(); it != feeds.end(); it++) {
		Feed *feed = (*it);
		for (uint32 j = 0; j < feed->ChannelCount(); j++) {
			Channel *channel = feed->ChannelAt(j);
			ItemList items = channel->FindItems(new NewItemSpecification(), true);
			
			unread += items.size();
		};
	};	

	return unread > 0;
};

void FeedIcon::HandleSettings(BMessage *settings) {
	int32 display = B_ERROR;
	if (settings->FindInt32("displayitems", &display) != B_OK) display = kMenuOptionAll;
	if (fMenuSpecification) delete fMenuSpecification;
	
	switch (display) {
		case kMenuOptionUnread: {
			fMenuSpecification = new UnreadItemSpecification();
		} break;
		
		case kMenuOptionCurrent: {
			fMenuSpecification = new CurrentItemSpecification();
		} break;
		
		case kMenuOptionUnreadCurrent: {
			fMenuSpecification = new AndSpecification<Item *>(new UnreadItemSpecification(), new UnreadItemSpecification());
		} break;

		case kMenuOptionAll:
		default: {
			fMenuSpecification = new AllItemSpecification();
		} break;
	};

};