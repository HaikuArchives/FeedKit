#include "TWindow.h"

#include <Debug.h>

#include <Application.h>
#include <be_apps/NetPositive/NetPositive.h>
#include <Box.h>
#include <Button.h>
#include <Dragger.h>
#include <File.h>
#include <FindDirectory.h>
#include <ListView.h>
#include <Menu.h>
#include <MenuBar.h>
#include <MenuField.h>
#include <MessageRunner.h>
#include <Messenger.h>
#include <OutlineListView.h>
#include <Path.h>
#include <PopUpMenu.h>
#include <Roster.h>
#include <ScrollView.h>
#include <Shelf.h>
#include <support/ClassInfo.h>
#include <View.h>

#ifdef B_ZETA_VERSION
#include <sys_apps/Tracker/Icons.h>
#endif

#include "ResizeView.h"
#include "IconMenu.h"
#include "Common/IconTextItem.h"
#include "CLV/ColumnListView.h"
#include "CLV/ColumnTypes.h"
#include "CLV/SortableBitmapColumn.h"
#include "RowTag.h"
#include "Settings.h"
#include "FeedRegistrationWindow.h"

#include <libfeedkit/Channel.h>
#include <libfeedkit/ChannelSpecification.h>
#include <libfeedkit/DownloadProgress.h>
#include <libfeedkit/Feed.h>
#include <libfeedkit/FeedKitConstants.h>
#include <libfeedkit/FeedListener.h>
#include <libfeedkit/Item.h>
#include <libfeedkit/ItemSpecification.h>
#include <libfeedkit/SettingsManager.h>
#include <libfeedkit/ObjectCache.h>

using namespace FeedKit;

//#pragma mark NewItemChannelSpecification

class NewItemChannelSpecification : public ChannelSpecification {
	public:
								NewItemChannelSpecification(void) {
									fNewItemSpecification = new NewItemSpecification();
								}
								
								~NewItemChannelSpecification(void) {
									delete fNewItemSpecification;
								};
	
		virtual bool			IsSatisfiedBy(Channel *channel) {
									ItemList newItems = channel->FindItems(fNewItemSpecification, false);
									
									return (newItems.empty() == false);
								};
	private:
		NewItemSpecification	*fNewItemSpecification;
};

//#pragma mark UnreadItemChannelSpecification

class UnreadItemChannelSpecification : public ChannelSpecification {
	public:
								UnreadItemChannelSpecification(void) {
									fUnreadItemSpecification = new UnreadItemSpecification();
								}
								
								~UnreadItemChannelSpecification(void) {
									delete fUnreadItemSpecification;
								};
	
		virtual bool			IsSatisfiedBy(Channel *channel) {
									ItemList unreadItems = channel->FindItems(fUnreadItemSpecification, false);
									
									return (unreadItems.empty() == false);
								};
	private:
		UnreadItemSpecification	*fUnreadItemSpecification;
};

//#pragma mark Types

#define CONTROL_PADDING ((float)(be_plain_font->StringWidth("X") * 2))

//#pragma mark Constants

const int32 kMsgListChanged = 'mflc';
const int32 kMsgItemChanged = 'mich';
const int32 kMsgMarkItemRead = 'mmir';
const int32 kMsgEnclosureInvoked = 'mein';
const int32 kMsgShowPreferences = 'mprf';
const int32 kMsgContentSelected = 'mcs ';
const int32 kMsgContentLinkSelected = 'mcls';
const int32 kMsgCancelDownload = 'mcd ';
const int32 kMsgChangeFeedSubscription = 'mcfs';
const int32 kMsgRefreshFeed = 'mrf ';
const int32 kMsgEnclosureDownload = 'med ';
const int32 kMsgEnclosureCancel = 'mcd ';
const int32 kMsgMarkFeedAsRead = 'mfar';
const int32 kMsgMarkChannelAsRead = 'mcar';
const int32 kMsgRegisterFeed = 'mre ';
const int32 kMsgRegisterFeedCancel = 'mrec';
const int32 kMsgRegisterFeedConfirm = 'mreo';


const char *kBlankPath = "/tmp/blank";
const char *kBlankURL = "file:///tmp/blank";

const bigtime_t kItemReadTime = 1000 * 1000 * 2; // 2 seconds

const float kNetPositiveHeaderSize = 27.0f;
const float kNetPositiveFooterSize = 14.0f;
const float kIconSize = 15.0f;
const float kIconColSize = 20.0f;
const float kFeedRowHeight = 20.0f;
const float kEnclosureRowHeight = 20.0f;

const int32 kFeedColNew = 0;
const int32 kFeedColUnread = 1;
const int32 kFeedColSubscribed = 2;
const int32 kFeedColName = 3;

const int32 kItemColNew = 0;
const int32 kItemColArchived = 1;
const int32 kItemColUnread = 2;
const int32 kItemColEnclosure = 3;
const int32 kItemColAuthor = 4;
const int32 kItemColTitle = 5;
const int32 kItemColDate = 6;

const int32 kEnclosureColURL = 0;
const int32 kEnclosureColMIME = 1;
const int32 kEnclosureColDescription = 2;
const int32 kEnclosureColProgress = 3;
const int32 kEnclosureColSize = 4;
const int32 kEnclosureColPath = 5;
const int32 kEnclosureColState = 6;

//#pragma mark Functions

const char *EnclosureStateName(EnclosureState state) {
	const char *name = NULL;
	
	switch (state) {
		case None: {
			name = "None";
		} break;
		
		case Queued: {
			name = "Queued";
		} break;
		
		case Downloading: {
			name = "Downloading";
		} break;
		
		case Cancelled: {
			name = "Cancelled";
		} break;
		
		case Error: {
			name = "Error";
		} break;
		
		case Completed: {
			name = "Completed";
		} break;
		
		default: {
			name = "Unknown";
		} break;
	};
	
	return name;
};

BPopUpMenu *FeedPopUpMenu(BHandler *target, Feed *feed) {
	BPopUpMenu *menu = new BPopUpMenu("FeedPopUpMenu", false, false);
	BMessage *toggleSubMsg = new BMessage(kMsgChangeFeedSubscription);
	const char *label = "Unsubscribe";
	
	toggleSubMsg->AddFlat("feed", feed);
	toggleSubMsg->AddBool("subscribe", feed->Subscribed() == false);
	if (feed->Subscribed() == false) label = "Subscribe";
	menu->AddItem(new BMenuItem(label, toggleSubMsg));

	if (feed->Subscribed() == true) {
		BMessage *refresh = new BMessage(kMsgRefreshFeed);
		refresh->AddFlat("feed", feed);
		menu->AddItem(new BMenuItem("Refresh", refresh));
		
		BMessage *markAsRead = new BMessage(kMsgMarkFeedAsRead);
		markAsRead->AddFlat("feed", feed);
		menu->AddItem(new BMenuItem("Mark all items as read", markAsRead));
	};
	 
	menu->SetTargetForItems(target);
	menu->SetFont(be_plain_font);

	return menu;
};

BPopUpMenu *ChannelPopUpMenu(BHandler *target, Feed *feed, Channel *channel) {
	BPopUpMenu *menu = new BPopUpMenu("ChannelPopUpMenu", false, false);

	if (feed->Subscribed() == true) {
		BMessage *markAsRead = new BMessage(kMsgMarkChannelAsRead);
		markAsRead->AddFlat("channel", channel);
		menu->AddItem(new BMenuItem("Mark all items as read", markAsRead));
	};

	menu->SetTargetForItems(target);
	menu->SetFont(be_plain_font);

	return menu;
};

BPopUpMenu *EnclosurePopUpMenu(BHandler *target, Feed */*feed*/, Channel */*channel*/, Item *item, Enclosure *enclosure) {
	BPopUpMenu *menu = new BPopUpMenu(enclosure->UUID(), false, false);

	if ((enclosure->State() == None) || (enclosure->State() == Cancelled)) {
		const char *label = "Download";
		BMessage *msgDownload = new BMessage(kMsgEnclosureDownload);
		msgDownload->AddFlat("item", item);
		msgDownload->AddFlat("enclosure", enclosure);
		
		if (enclosure->State() == Cancelled) label = "Resume";
		menu->AddItem(new BMenuItem(label, msgDownload));
	};
	if ((enclosure->State() == Queued) || (enclosure->State() == Downloading)) {
		BMessage *msgCancel = new BMessage(kMsgEnclosureCancel);
		msgCancel->AddFlat("item", item);
		msgCancel->AddFlat("enclosure", enclosure);
		
		menu->AddItem(new BMenuItem("Cancel", msgCancel));
	};

	menu->SetTargetForItems(target);
	menu->SetFont(be_plain_font);
	
	return menu;
};

//#pragma mark Constructor

TWindow::TWindow(void)
	: BWindow(BRect(25, 25, 660, 685), "Anchorman", B_DOCUMENT_WINDOW, B_ASYNCHRONOUS_CONTROLS | B_OUTLINE_RESIZE),
	fSelectedChannel(NULL),
	fSelectedItem(NULL),
	fItemReadRunner(NULL) {

	BRect frame = Bounds();

	BMenuBar *bar = new BMenuBar(frame, "MenuBar");
	BMenu *menu = new BMenu("App");
	TIconMenu *appMenu = new TIconMenu(menu);
	
	menu->AddItem(new BMenuItem("Register Feed" B_UTF8_ELLIPSIS, new BMessage(kMsgRegisterFeed)));
	menu->AddItem(new BMenuItem("Preferences" B_UTF8_ELLIPSIS, new BMessage(kMsgShowPreferences)));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem("Quit", new BMessage(B_QUIT_REQUESTED)));

	bar->AddItem(appMenu);
	AddChild(bar);

	frame.top = bar->Frame().bottom + 1.0f;
	BView *parent = new BView(frame, "ParentView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	AddChild(parent);
		
#if B_BEOS_VERSION > B_BEOS_VERSION_5
	parent->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	parent->SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	parent->SetHighColor(ui_color(B_PANEL_TEXT_COLOR));
#else
	parent->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	parent->SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	parent->SetHighColor(0, 0, 0, 0);
#endif

	// Left - right splitter + child controls
	BRect lrResizer = parent->Bounds();

	BRect feedRect(lrResizer);
	feedRect.OffsetTo(B_ORIGIN);
	feedRect.InsetBy(CONTROL_PADDING, CONTROL_PADDING);
	feedRect.right *= 0.25;

	fChannelList = new BColumnListView(feedRect, "fChannelList", B_FOLLOW_TOP_BOTTOM, B_WILL_DRAW, B_FANCY_BORDER);
	fChannelList->AddColumn(new SortableBitmapColumn("New", kIconColSize, kIconColSize, kIconColSize, B_ALIGN_CENTER), kFeedColNew);
	fChannelList->AddColumn(new SortableBitmapColumn("Unread", kIconColSize, kIconColSize, kIconColSize, B_ALIGN_CENTER), kFeedColUnread);
	fChannelList->AddColumn(new SortableBitmapColumn("Subscribed", kIconColSize, kIconColSize, kIconColSize, B_ALIGN_CENTER), kFeedColSubscribed);
	fChannelList->AddColumn(new BStringColumn("Name", 100.0f, be_plain_font->StringWidth("Name") + CONTROL_PADDING, 300.0f, B_TRUNCATE_END), kFeedColName);
	fChannelList->SetSelectionMode(B_SINGLE_SELECTION_LIST);
	fChannelList->SetSelectionMessage(new BMessage(kMsgListChanged));
	fChannelList->SetTarget(this);

	BRect boxRect(lrResizer);
	boxRect.OffsetTo(B_ORIGIN);
	boxRect.InsetBy(CONTROL_PADDING, CONTROL_PADDING);
	boxRect.left = feedRect.right + (CONTROL_PADDING * 2);
		
	fBox = new BBox(boxRect, "fBox", B_FOLLOW_TOP_BOTTOM);

	ResizeView *rvLeftRight = new ResizeView(fChannelList, fBox, lrResizer, "LeftRightResize", B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	rvLeftRight->AddChild(fChannelList);
	rvLeftRight->AddChild(fBox);

	parent->AddChild(rvLeftRight);
	rvLeftRight->SetResizeMode(RESIZE_MODE_LOCK_VIEW_1);

	// Top / Bottom controls
	BRect tbResizer = fBox->Bounds();
	tbResizer.InsetBy(CONTROL_PADDING, CONTROL_PADDING);
	
	BRect itemRect = tbResizer;
	itemRect.OffsetTo(B_ORIGIN);
	itemRect.bottom *= 0.25;
	itemRect.bottom -= CONTROL_PADDING;

	fItemList = new BColumnListView(itemRect, "fItemList", B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW, B_FANCY_BORDER);
	fItemList->AddColumn(new SortableBitmapColumn("New", kIconColSize, kIconColSize, kIconColSize, B_ALIGN_CENTER), kItemColNew);
	fItemList->AddColumn(new SortableBitmapColumn("Archived", kIconColSize, kIconColSize, kIconColSize, B_ALIGN_CENTER), kItemColArchived);
	fItemList->AddColumn(new SortableBitmapColumn("Unread", kIconColSize, kIconColSize, kIconColSize, B_ALIGN_CENTER), kItemColUnread);
	fItemList->AddColumn(new SortableBitmapColumn("Enclosures", kIconColSize, kIconColSize, kIconColSize, B_ALIGN_CENTER), kItemColEnclosure);
	fItemList->AddColumn(new BStringColumn("Author", 100.0f, be_plain_font->StringWidth("Author") + CONTROL_PADDING, 300.0f, B_TRUNCATE_END), kItemColAuthor);
	fItemList->AddColumn(new BStringColumn("Title", 100.0f, be_plain_font->StringWidth("Title") + CONTROL_PADDING, 300.0f, B_TRUNCATE_END), kItemColTitle);
	fItemList->AddColumn(new BDateColumn("Date", 100.0f, be_plain_font->StringWidth("Date") + CONTROL_PADDING, 300.0f), kItemColDate);
	fItemList->SetSelectionMessage(new BMessage(kMsgItemChanged));
	fItemList->SetSelectionMode(B_SINGLE_SELECTION_LIST);

	BRect contentEnclosureBoxRect = tbResizer;
	contentEnclosureBoxRect.OffsetTo(B_ORIGIN);
	contentEnclosureBoxRect.top = itemRect.bottom + (CONTROL_PADDING * 2);

	fContentEnclosureBox = new BBox(contentEnclosureBoxRect, "fContentEnclosureBox", B_FOLLOW_LEFT_RIGHT);
	fContentEnclosureBox->SetLabel("Item Details");
	
	BRect contentEnclosureRect = contentEnclosureBoxRect;
	contentEnclosureRect.OffsetTo(B_ORIGIN);
	contentEnclosureRect.InsetBy(CONTROL_PADDING, CONTROL_PADDING);

	BRect contentBoxRect = contentEnclosureRect;
	contentBoxRect.OffsetTo(B_ORIGIN);
	contentBoxRect.bottom = (contentBoxRect. bottom / 2) - CONTROL_PADDING;
	BBox *contentBox = new BBox(contentBoxRect, "ContentBox", B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW);

	fContentMenu = new BMenu("fContentMenu");
	fContentMenu->SetLabelFromMarked(true);
	
	BMenuField *contentMenuField = new BMenuField(contentBoxRect, "ContentMenuField", NULL, fContentMenu);
	contentBox->SetLabel(contentMenuField);

	BRect shelfRect = contentBoxRect;
	shelfRect.OffsetTo(B_ORIGIN);
	shelfRect.top += CONTROL_PADDING;
	shelfRect.InsetBy(CONTROL_PADDING, CONTROL_PADDING);

	BView *shelfView = new BView(shelfRect, "ShelfView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	BShelf *shelf = new BShelf(shelfView, false);
	shelf->SetTypeEnforced(false);
	shelf->SetAllowsDragging(false);

	contentBox->AddChild(shelfView);

	BRect enclosureBoxRect = contentEnclosureRect;
	enclosureBoxRect.OffsetTo(B_ORIGIN);
	enclosureBoxRect.top = (enclosureBoxRect.bottom / 2) + CONTROL_PADDING;
	
	BBox *enclosureBox = new BBox(enclosureBoxRect, "EnclosureBox", B_FOLLOW_LEFT_RIGHT);
	enclosureBox->SetLabel("Enclosures");

	BRect enclosureRect = enclosureBoxRect;
	enclosureRect.OffsetTo(B_ORIGIN);
	enclosureRect.InsetBy(CONTROL_PADDING, CONTROL_PADDING);

	fEnclosureList = new BColumnListView(enclosureRect, "fEnclosureList", B_FOLLOW_ALL_SIDES, B_WILL_DRAW, B_FANCY_BORDER);
	fEnclosureList->AddColumn(new BStringColumn("URL", 100.0f, be_plain_font->StringWidth("URL") + CONTROL_PADDING, 300.0f, B_TRUNCATE_END), kEnclosureColURL);
	fEnclosureList->AddColumn(new BStringColumn("MIME", 100.0f, be_plain_font->StringWidth("MIME") + CONTROL_PADDING, 300.0f, B_TRUNCATE_END), kEnclosureColMIME);
	fEnclosureList->AddColumn(new BStringColumn("Description", 100.0f, be_plain_font->StringWidth("Description") + CONTROL_PADDING, 300.0f, B_TRUNCATE_END), kEnclosureColDescription);
	fEnclosureList->AddColumn(new GraphColumn("Progress", 100.0f, be_plain_font->StringWidth("Progress") + CONTROL_PADDING, 300.0f, B_ALIGN_LEFT), kEnclosureColProgress);
	fEnclosureList->AddColumn(new BSizeColumn("Size", 100.0f, be_plain_font->StringWidth("Size") + CONTROL_PADDING, 300.0f), kEnclosureColSize);
	fEnclosureList->AddColumn(new BStringColumn("Path", 100.0f, be_plain_font->StringWidth("Path") + CONTROL_PADDING, 300.0f, B_TRUNCATE_END), kEnclosureColPath);
	fEnclosureList->AddColumn(new BStringColumn("State", 100.0f, be_plain_font->StringWidth("State") + CONTROL_PADDING, 300.0f, B_TRUNCATE_END), kEnclosureColState);
	fEnclosureList->SetInvocationMessage(new BMessage(kMsgEnclosureInvoked));
	
	enclosureBox->AddChild(fEnclosureList);
	
	ResizeView *rvContentEnclosure = new ResizeView(contentBox, enclosureBox, contentEnclosureRect, "ContentEnclosureResize", B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	rvContentEnclosure->SetResizeMode(RESIZE_MODE_LOCK_VIEW_2);
	rvContentEnclosure->SetVertical(false);
	rvContentEnclosure->AddChild(contentBox);
	rvContentEnclosure->AddChild(enclosureBox);

	fContentEnclosureBox->AddChild(rvContentEnclosure);

	ResizeView *rvItemContentEnclosure = new ResizeView(fItemList, fContentEnclosureBox, tbResizer, "TopBottomResize", B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	rvItemContentEnclosure->SetResizeMode(RESIZE_MODE_LOCK_VIEW_1);
	rvItemContentEnclosure->SetVertical(false);
	rvItemContentEnclosure->AddChild(fItemList);
	rvItemContentEnclosure->AddChild(fContentEnclosureBox);
	
	fBox->AddChild(rvItemContentEnclosure);
	
	BRect replRect = shelfRect;
	replRect.bottom += kNetPositiveHeaderSize + kNetPositiveFooterSize;

	BMessage netMsg(B_ARCHIVED_OBJECT);
	netMsg.AddString("add_on", "application/x-vnd.Be-NPOS");
	netMsg.AddInt32("version", 2);
	netMsg.AddBool("openAsText", false);
	netMsg.AddInt32("encoding", 14);
	netMsg.AddString("class", "NPBaseView");
	netMsg.AddString("_name", "fNetPositive");
	netMsg.AddRect("_frame", replRect);
	netMsg.AddInt32("_resize_mode", 4660);
	netMsg.AddInt32("_flags", 603979776);
	netMsg.AddInt32("be:actions", 1145328205);
	netMsg.AddInt32("buttons", 1);

	shelf->AddReplicant(&netMsg, BPoint(0, - kNetPositiveHeaderSize));
	
	shelf->ReplicantAt(0, &fNetPositive);
	fNetPositive->SetResizingMode(B_FOLLOW_ALL_SIDES);

	Init();
};

TWindow::TWindow(BMessage *archive)
	: BWindow(archive),
	fSelectedChannel(NULL),
	fSelectedItem(NULL),
	fItemReadRunner(NULL) {
	
	fChannelList = reinterpret_cast<BColumnListView *>(FindView("fChannelList"));
	fBox = reinterpret_cast<BBox *>(FindView("fBox"));
	fItemList = reinterpret_cast<BColumnListView *>(FindView("fItemList"));
	fContentEnclosureBox = reinterpret_cast<BBox *>(FindView("fContentEnclosureBox"));
	fNetPositive = FindView("fNetPositive");
	fEnclosureList = reinterpret_cast<BColumnListView *>(FindView("fEnclosureList"));
	
	ASSERT_WITH_MESSAGE(fChannelList != NULL, "");
	ASSERT_WITH_MESSAGE(fBox != NULL, "");
	ASSERT_WITH_MESSAGE(fItemList != NULL, "");
	ASSERT_WITH_MESSAGE(fContentEnclosureBox != NULL, "");
	ASSERT_WITH_MESSAGE(fNetPositive != NULL, "");
	ASSERT_WITH_MESSAGE(fEnclosureList != NULL, "");

	BMenuField *menuField = reinterpret_cast<BMenuField *>(FindView("ContentMenuField"));
	ASSERT_WITH_MESSAGE(menuField != NULL, "");

	fContentMenu = menuField->Menu();
	ASSERT_WITH_MESSAGE(fContentMenu != NULL, "");

	fContentMenu->SetLabelFromMarked(true);
	fContentMenu->ResizeToPreferred();
	
	fNetPositive->SetResizingMode(B_FOLLOW_ALL_SIDES);
	
	Init();
};

TWindow::~TWindow(void) {
	while (CountChildren() > 0) {
		BView *child = ChildAt(0L);
		child->RemoveSelf();
		
		delete child;
	}
	
	delete fCache;
};

//#pragma mark Window Hooks

bool TWindow::QuitRequested(void) {
	BMessenger(be_app).SendMessage(B_QUIT_REQUESTED);

	BMessage settings = fSettingsManager->Settings();

	BMessage archive;
	Archive(&archive, true);

	if (settings.ReplaceMessage(kSettingsMessageName, &archive) != B_OK) {
		settings.AddMessage(kSettingsMessageName, &archive);
	};
	
	fSettingsManager->Settings(&settings);

	return true;
};

void TWindow::MessageReceived(BMessage *msg) {
	switch (msg->what) {
		case kMsgListChanged: {
			ClearItemRunner();
			ClearItemList();
			ClearEnclosureList();
			ClearContentView();

			row_t *row = reinterpret_cast<row_t *>(fChannelList->CurrentSelection());
			if (row == NULL) return;

			Channel *channel = fCache->GetChannel(row->Tag().String());
			if (channel == NULL) return;
			
			fBox->SetLabel(channel->Title());
			fSelectedChannel = channel;
		
			for (uint32 j = 0; j < channel->ItemCount(); j++) {
				Item *item = channel->ItemAt(j);
				
				time_t date = item->Date();
				BBitmap *iconEnclosure = NULL;
				BBitmap *iconNew = NULL;
				BBitmap *iconArchived = NULL;
				BBitmap *iconUnread = NULL;
				if (item->EnclosureCount() > 0) iconEnclosure = fIconAttachment;
				if (item->New() == true) iconNew = fIconNew;
				if (item->Current() == false) iconArchived = fIconArchived;
				if (item->Read() == false) iconUnread = fIconUnread;
	
				row_t *row = new row_t(item->UUID());
				row->SetField(new SortableBitmapField(iconNew, iconNew == NULL ? 0 : 1), kItemColNew);
				row->SetField(new SortableBitmapField(iconArchived, iconArchived == NULL ? 0 : 1), kItemColArchived);
				row->SetField(new SortableBitmapField(iconUnread, iconUnread == NULL ? 0 : 1), kItemColUnread);
				row->SetField(new SortableBitmapField(iconEnclosure, item->EnclosureCount()), kItemColEnclosure);
				row->SetField(new BStringField(item->Author()), kItemColAuthor);
				row->SetField(new BStringField(item->Title()), kItemColTitle);
				row->SetField(new BDateField(&date), kItemColDate);
	
				fItemList->AddRow(row);
			};
		} break;

		case kMsgShowPreferences: {
			be_roster->Launch("application/x-vnd.beclan-FeedKit-Prefs");
		} break;

		case kMsgItemChanged: {
			ClearItemRunner();
			ClearEnclosureList();
			ClearContentView();
			
			row_t *row = reinterpret_cast<row_t * >(fItemList->CurrentSelection());
			if (row == NULL) return;
			
			Item *item = fCache->GetItem(row->Tag().String());
			if (item->Read() == false) {
				BMessage markRead(kMsgMarkItemRead);
				fItemReadRunner = new BMessageRunner(BMessenger(this), &markRead, kItemReadTime, 1);
			};
			
			fContentEnclosureBox->SetLabel(item->Title());
			fSelectedItem = item;
									
			for (uint32 i = 0; i < item->EnclosureCount(); i++) {
				Enclosure *enclosure = item->EnclosureAt(i);
				const DownloadProgress *progress = enclosure->Progress();
				entry_ref ref = enclosure->LocalRef();
				BString path = "";
				int32 percentProgress = 0;

				if (enclosure->State() != None) path = BPath(&ref).Path();
				if (progress != NULL) percentProgress = (int32)(progress->PercentageComplete() * 100);
			
				row_t *row = new row_t(kEnclosureRowHeight, enclosure->UUID());
				row->SetField(new BStringField(enclosure->URL()), kEnclosureColURL);
				row->SetField(new BStringField(enclosure->MIME()), kEnclosureColMIME);
				row->SetField(new BStringField(enclosure->Description()), kEnclosureColDescription);
				row->SetField(new BIntegerField(percentProgress), kEnclosureColProgress);
				row->SetField(new BSizeField(enclosure->Size()), kEnclosureColSize);
				row->SetField(new BStringField(path.String()), kEnclosureColPath);
				row->SetField(new BStringField(EnclosureStateName(enclosure->State())), kEnclosureColState);	
				fEnclosureList->AddRow(row);

				Channel *channel = item->ParentChannel();
				Feed *feed = channel->ParentFeed();
				BPopUpMenu *menu = EnclosurePopUpMenu(this, feed, channel, item, enclosure);
				if (menu->CountItems() == 0) {
					delete menu;
					menu = NULL;
				};
				row->SetMenu(menu);
			};
			
			for (uint32 i = 0; i < item->ContentCount(); i++) {
				Content *content = item->ContentAt(i);
				BString label = content->MIMEType();
				if (label.Length() == 0) {
					label = "Unknown (";
					label << strlen(content->Text()) << " character(s))";
				};
				
				BMessage *contentSelected = new BMessage(kMsgContentSelected);
				contentSelected->AddPointer("content", content);
				
				BMenuItem *contentItem = new BMenuItem(label.String(), contentSelected);			
				fContentMenu->AddItem(contentItem);
			};
			
			bool hasSeparator = false;
			
			if (strlen(item->Link()) > 0) {
				if (fContentMenu->CountItems() > 0) {
					fContentMenu->AddSeparatorItem();
					hasSeparator = true;
				} 
				
				BMessage *linkSelected = new BMessage(kMsgContentLinkSelected);
				linkSelected->AddString("url", item->Link());
				
				BMenuItem *linkItem = new BMenuItem("Read more" B_UTF8_ELLIPSIS, linkSelected);
				fContentMenu->AddItem(linkItem);
			};
			
			if (strlen(item->Comments()) > 0) {
				if ((hasSeparator == false) && (fContentMenu->CountItems() > 0)) {
					fContentMenu->AddSeparatorItem();
					hasSeparator = true;
				};

				BMessage *commentSelected = new BMessage(kMsgContentLinkSelected);
				commentSelected->AddString("url", item->Comments());
				
				BMenuItem *commentItem = new BMenuItem("Comments" B_UTF8_ELLIPSIS, commentSelected);
				fContentMenu->AddItem(commentItem);			
			};
						
			fContentMenu->SetTargetForItems(this);
			fContentMenu->ResizeToPreferred();
			
			if (fContentMenu->CountItems() > 0) fContentMenu->ItemAt(0)->Invoke();
		} break;
	
		case kMsgContentSelected: {
			Content *content = NULL;
			
			if (msg->FindPointer("content", reinterpret_cast<void **>(&content)) != B_OK) {
				VisitURL(kBlankURL);
				return;
			};
		
			BPath path("/tmp");
			path.Append(content->UUID());
		
			BFile file(path.Path(), B_READ_WRITE | B_CREATE_FILE | B_FAIL_IF_EXISTS);
			if (file.InitCheck() == B_OK) {
				BString buffer = content->Text();
			
				if (strcmp(content->MIMEType(), "text/html") != 0) {
					buffer.Prepend("<html>");
					buffer.Append("</html>");
				};
				
				file.Write(buffer.String(), buffer.Length());
				file.Unset();
			};
			
			BString url = "file://";
			url << path.Path();
			VisitURL(url.String());			
		} break;
	
		case kMsgContentLinkSelected: {
			const char *URL = NULL;
			if (msg->FindString("url", &URL) != B_OK) return;
			
			VisitURL(URL);
		} break;
	
		case kMsgMarkItemRead: {
			if (fSelectedItem != NULL) fFeedListener->MarkItemRead(fSelectedItem);
			
			ClearItemRunner();
		} break;

		case kMsgChangeFeedSubscription: {
			Feed feed;
			bool subscribe = false;
		
			if (msg->FindFlat("feed", &feed) != B_OK) return;
			if (msg->FindBool("subscribe", &subscribe) != B_OK) subscribe = (feed.Subscribed() == false);

			fFeedListener->ChangeFeedSubscription(&feed, subscribe);
		} break;
	
		case kMsgRefreshFeed: {
			Feed feed;		
			if (msg->FindFlat("feed", &feed) != B_OK) return;
	
			fFeedListener->RefreshFeed(&feed);
		} break;
	
		case kMsgEnclosureDownload: {
			Item item;
			Enclosure enclosure;
			
			if (msg->FindFlat("item", &item) != B_OK) return;
			if (msg->FindFlat("enclosure", &enclosure) != B_OK) return;
			
			entry_ref ref;
			BMessage settings = fSettingsManager->Settings();
			if (settings.FindRef("enclosure_path", &ref) != B_OK) {
				BPath homePath;
				find_directory(B_USER_DIRECTORY, &homePath);
				get_ref_for_path(homePath.Path(), &ref);
			};
			
			BPath path(&ref);
			BString url = enclosure.URL();
			int32 slash = url.IFindLast("/");

			if (slash != B_ERROR) {
				url.Remove(0, slash + 1);
			};
			
			path.Append(url.String());

			fFeedListener->DownloadEnclosure(&item, &enclosure, path.Path());
		} break;
	
		case kMsgEnclosureCancel: {
			Item item;
			Enclosure enclosure;
			
			if (msg->FindFlat("item", &item) != B_OK) return;
			if (msg->FindFlat("enclosure", &enclosure) != B_OK) return;
			
			fFeedListener->CancelEnclosureDownload(&item, &enclosure);
		} break;
	
		case kMsgEnclosureInvoked: {
			row_t *row = reinterpret_cast<row_t * >(fEnclosureList->CurrentSelection());
			if (row == NULL) return;

			Enclosure *enclosure = fCache->GetEnclosure(row->Tag().String());
			if (enclosure == NULL) return;
			
			if (enclosure->State() != FeedKit::Completed) return;
			
			entry_ref ref = enclosure->LocalRef();
			status_t result = be_roster->Launch(&ref);

			fprintf(stderr, "TWindow::MessageReceived(): Launching Enclosure - %s: %s (%i)\n", ref.name, strerror(result), result);
		} break;
	
		case kMsgMarkFeedAsRead: {
			Feed feed;
			if (msg->FindFlat("feed", &feed) != B_OK) return;
			
			for (uint32 i = 0; i < feed.ChannelCount(); i++) {
				Channel *channel = feed.ChannelAt(i);
				
				for (uint32 j = 0; j < channel->ItemCount(); j++) {
					Item *item = channel->ItemAt(j);
					if (item->Read() == false) fFeedListener->MarkItemRead(item);
				};
			};
		} break;
	
		case kMsgMarkChannelAsRead: {
			Channel channel;
			if (msg->FindFlat("channel", &channel) != B_OK) return;
		
			for (uint32 i = 0; i < channel.ItemCount(); i++) {
				Item *item = channel.ItemAt(i);
				if (item->Read() == false) fFeedListener->MarkItemRead(item);
			};
		} break;
	
		case kMsgRegisterFeed: {
			if (fSubscribe == NULL) {
				fSubscribe = new FeedRegistrationWindow(new BMessenger(this),
					new BMessage(kMsgRegisterFeedCancel), new BMessage(kMsgRegisterFeedConfirm));
			};
			
			fSubscribe->Show();
			fSubscribe->Activate(true);
		} break;

		case kMsgRegisterFeedConfirm: {
			const char *url = NULL;
			const char *name = NULL;

			if (msg->FindString("url", &url) != B_OK) return;
			if (msg->FindString("name", &name) != B_OK) name = "";

			fFeedListener->RegisterFeed(url, name);
			
			fSubscribe = NULL;
		} break;
		
		case kMsgRegisterFeedCancel: {
			fSubscribe = NULL;
		} break;
	
		default: {
			BWindow::MessageReceived(msg);
		} break;
	};
};

//#pragma mark BArchivable Hooks

status_t TWindow::Archive(BMessage *archive, bool deep = true) const {
	return BWindow::Archive(archive, deep);
};

BArchivable *TWindow::Instantiate(BMessage *archive) {
	BArchivable *instance = NULL;
	
	if (validate_instantiation(archive, "TWindow") == true) {
	   instance = new TWindow(archive);
	};

	return instance;
};

//#pragma mark FeedHandler Hooks

void TWindow::ServerStarted(void) {
	Lock();

	fCache->Clear();
	RebuildFeedList();
	
	Unlock();
};

void TWindow::ServerShutdown(void) {
	Lock();
	
	fCache->Clear();
	RebuildFeedList();
	
	Unlock();
};

void TWindow::FeedRegistered(Feed */*feed*/) {
	Lock();

	RebuildFeedList();
	
	Unlock();
};

void TWindow::FeedSubscriptionChanged(Feed *feed) {
	Lock();

	fCache->AddFeed(feed);
	feed_row_t::iterator rIt = fFeedRow.find(feed->UUID());
	if (rIt != fFeedRow.end()) {
		fprintf(stderr, "TWindow::FeedSubscriptionChanged(%s) - Updating row to be %s\n", feed->DisplayName(), feed->Subscribed() ? "subscribed" : "unsubscribed");
		row_t *row = rIt->second;

		row->SetField(new SortableBitmapField(feed->Subscribed() == true ? fIconSubscribed : NULL, feed->Subscribed() ? 1 : 0), kFeedColSubscribed);
		fChannelList->UpdateRow(row);
		
		row->SetMenu(FeedPopUpMenu(this, feed));
		
		for (int32 j = 0; j < fChannelList->CountRows(row); j++) {
			row_t *channelRow = reinterpret_cast<row_t *>(fChannelList->RowAt(j, row));

			channelRow->SetField(new SortableBitmapField(feed->Subscribed() == true ? fIconSubscribed : NULL, feed->Subscribed() ? 1 : 0), kFeedColSubscribed);
			fChannelList->UpdateRow(channelRow);
			
			Channel *channel = fCache->GetChannel(channelRow->Tag().String());
			BPopUpMenu *chanMenu = ChannelPopUpMenu(this, feed, channel);
			if (chanMenu->CountItems() == 0) {
				delete chanMenu;
				chanMenu = NULL;
			};
			channelRow->SetMenu(chanMenu);
		};
	} else {
		fprintf(stderr, "TWindow::FeedSubscriptionChanged(%s) - Could not find row\n", feed->DisplayName());
	};
	
	Unlock();
};

void TWindow::ChannelUpdated(Feed */*feed*/, Channel */*channel*/) {
	Lock();
	
	RebuildFeedList();
	
	Unlock();
};

void TWindow::ItemRead(Feed *feed, Channel *channel, Item *item) {
	Lock();
	
	fCache->AddFeed(feed);

	if (fSelectedChannel != NULL) {
		// Mark the item as read			
		for (int32 i = 0; i < fItemList->CountRows(); i++) {
			row_t *row = reinterpret_cast<row_t *>(fItemList->RowAt(i));
		
			if (strcmp(row->Tag().String(), item->UUID()) == 0) {
				row->SetField(new SortableBitmapField(NULL, 0), kItemColUnread);
				fItemList->UpdateRow(row);
				break;
			};
		};
	};

	ChannelSpecification *newItemChanSpec = new NewItemChannelSpecification();
	ChannelSpecification *unreadItemChanSpec = new UnreadItemChannelSpecification();
	ItemSpecification *newItemSpec = new NewItemSpecification();
	ItemSpecification *unreadItemSpec = new UnreadItemSpecification();
	bool found = false;

	for (int32 i = 0; i < fChannelList->CountRows(); i++) {
		row_t *feedRow = reinterpret_cast<row_t *>(fChannelList->RowAt(i));
		
		for (int32 j = 0; j < fChannelList->CountRows(feedRow); j++) {
			row_t *row = reinterpret_cast<row_t *>(fChannelList->RowAt(j, feedRow));
			
			if (row == NULL) continue;
			if (strcmp(row->Tag().String(), channel->UUID()) != 0) continue;
	
			ItemList newItems = channel->FindItems(newItemSpec, false);
			ItemList unreadItems = channel->FindItems(unreadItemSpec, false);
			
			row->SetField(new SortableBitmapField(newItems.empty() ? NULL : fIconNew, newItems.size()), kFeedColNew);
			row->SetField(new SortableBitmapField(unreadItems.empty() ? NULL : fIconUnread, unreadItems.size()), kFeedColUnread);
			fChannelList->UpdateRow(row);
			
			ChannelList newChanItems = feed->FindChannels(newItemChanSpec, false);
			ChannelList unreadChanItems = feed->FindChannels(unreadItemChanSpec, false);

			feedRow->SetField(new SortableBitmapField(newChanItems.empty() ? NULL : fIconNew, newChanItems.size()), kFeedColNew);
			feedRow->SetField(new SortableBitmapField(unreadChanItems.empty() ? NULL : fIconUnread, unreadChanItems.size()), kFeedColUnread);
			fChannelList->UpdateRow(feedRow);
			
			found = true;
			break;
		};
		
		if (found == true) break;
	};
	
	delete newItemSpec;
	delete unreadItemSpec;
	delete newItemChanSpec;
	delete unreadItemChanSpec;

	Unlock();
};

void TWindow::EnclosureDownloadStarted(FeedKit::Feed *feed, FeedKit::Item *item, FeedKit::Enclosure *enclosure) {
	fCache->AddFeed(feed);
	UpdateEnclosureStatus(item, enclosure, 0, enclosure->LocalRef());
};

void TWindow::EnclosureDownloadProgress(FeedKit::Feed *feed, FeedKit::Item *item, FeedKit::Enclosure *enclosure) {
	fCache->AddFeed(feed);

	if (enclosure->Progress() != NULL) {
		int32 percentage = 0;
		const DownloadProgress *progress = enclosure->Progress();
		
		if (progress != NULL) percentage = (int32)(progress->PercentageComplete() * 100);
	
		UpdateEnclosureStatus(item, enclosure, percentage, enclosure->LocalRef());
	};
};

void TWindow::EnclosureDownloadFinished(FeedKit::Feed *feed, FeedKit::Item *item, FeedKit::Enclosure *enclosure) {
	fCache->AddFeed(feed);
	
	UpdateEnclosureStatus(item, enclosure, 100, enclosure->LocalRef());
};

void TWindow::EnclosureDownloadError(FeedKit::Feed *feed, FeedKit::Item */*item*/, FeedKit::Enclosure */*enclosure*/, FeedKit::ErrorDetails */*error*/) {
	fCache->AddFeed(feed);
};

void TWindow::EnclosureDownloadStatusChanged(FeedKit::Feed *feed, FeedKit::Item *item, FeedKit::Enclosure *enclosure) {
	fCache->AddFeed(feed);

	int32 percentage = 0;
	const DownloadProgress *progress = enclosure->Progress();
	
	if (progress != NULL) percentage = (int32)(progress->PercentageComplete() * 100);
	
	UpdateEnclosureStatus(item, enclosure, percentage, enclosure->LocalRef());
};

//#pragma mark Private

void TWindow::Init(void) {
	fSubscribe = NULL;
	fCache = new ObjectCache();

	fFeedListener = new FeedKit::FeedListener();
	fFeedListener->StartListening();
	fFeedListener->AddHandler(this);

	fSettingsManager = new FeedKit::Settings::SettingsManager(FeedKit::Settings::AppTypes::SettingClient, kSettingsAppName);
	fSettingsManager->SetTarget(this);

	BMessage settings;
	BPath homePath;
	entry_ref homeRef;
	find_directory(B_USER_DIRECTORY, &homePath);
	get_ref_for_path(homePath.Path(), &homeRef);
	
	BMessage encPath;
	encPath.AddInt32("type", B_REF_TYPE);
	encPath.AddString("name", "enclosure_path");
	encPath.AddString("label", "Path to save Enclosures to");
	encPath.AddString("help", "Controls where Enclosures will be saved to by default");
	encPath.AddInt32("display_type", FeedKit::Settings::DirectoryPickerSingle);
	encPath.AddRef("default_value", &homeRef);

	settings.AddMessage("setting", &encPath);
	
	fSettingsManager->Template(&settings, kAppSignature);

	fSelectedChannel = NULL;
	fSelectedItem = NULL;
	fItemReadRunner = NULL;

	BPath path;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) == B_OK) {
		path.Append("BeClan/FeedKit/Icons/");
		
		fIconAttachment = ReadNodeIcon(BString(path.Path()).Append("/anchorman-attachment.png"), kIconSize);
		fIconNew = ReadNodeIcon(BString(path.Path()).Append("/anchorman-new.png"), kIconSize);
		fIconArchived = ReadNodeIcon(BString(path.Path()).Append("/anchorman-archived.png"), kIconSize);
		fIconUnread = ReadNodeIcon(BString(path.Path()).Append("/anchorman-unread.png"), kIconSize);
		fIconSubscribed = ReadNodeIcon(BString(path.Path()).Append("/anchorman-subscribed.png"), kIconSize);
	};
	
	fEnclosureList->SetTarget(this);
	
	AddShortcut('R', 0, new BMessage(kMsgMarkItemRead));

	ClearItemList();
	ClearContentView();
	ClearEnclosureList();

	RebuildFeedList();
};

void TWindow::RebuildFeedList() {
	fSelectedItem = NULL;

	ClearItemRunner();
	ClearFeedList();

	feed_list_t feeds = fFeedListener->Feeds();
	ChannelSpecification *newItemChanSpec = new NewItemChannelSpecification();
	ChannelSpecification *unreadItemChanSpec = new UnreadItemChannelSpecification();
	ItemSpecification *newItemSpec = new NewItemSpecification();
	ItemSpecification *unreadItemSpec = new UnreadItemSpecification();

	for (feed_list_t::iterator it = feeds.begin(); it != feeds.end(); it++) {
		Feed *feed = (*it);
		fCache->AddFeed(feed);
		
		ChannelList newChanItems = feed->FindChannels(newItemChanSpec, false);
		ChannelList unreadChanItems = feed->FindChannels(unreadItemChanSpec, false);

		row_t *feedRow = new row_t(kFeedRowHeight, NULL);		
		feedRow->SetField(new SortableBitmapField(newChanItems.empty() ? NULL : fIconNew, newChanItems.size()), kFeedColNew);
		feedRow->SetField(new SortableBitmapField(unreadChanItems.empty() ? NULL : fIconUnread, unreadChanItems.size()), kFeedColUnread);
		feedRow->SetField(new SortableBitmapField(feed->Subscribed() ? fIconSubscribed : NULL, feed->Subscribed() ? 1 : 0), kFeedColSubscribed);
		feedRow->SetField(new BStringField(feed->DisplayName()), kFeedColName);
		fChannelList->AddRow(feedRow);

		feedRow->SetMenu(FeedPopUpMenu(this, feed));

		fFeedRow[feed->UUID()] = feedRow;

		for (uint32 j = 0; j < feed->ChannelCount(); j++) {
			Channel *channel = feed->ChannelAt(j);
			ItemList newItems = channel->FindItems(newItemSpec, false);
			ItemList unreadItems = channel->FindItems(unreadItemSpec, false);
			
			row_t *row = new row_t(kFeedRowHeight, channel->UUID());
			row->SetField(new SortableBitmapField(newItems.empty() ? NULL : fIconNew, newItems.size()), kFeedColNew);
			row->SetField(new SortableBitmapField(unreadItems.empty() ? NULL : fIconUnread, unreadItems.size()), kFeedColUnread);
			row->SetField(new SortableBitmapField(feed->Subscribed() ? fIconSubscribed : NULL, feed->Subscribed() ? 1 : 0), kFeedColSubscribed);
			row->SetField(new BStringField(channel->Title()), kFeedColName);		
			fChannelList->AddRow(row, feedRow);

			BPopUpMenu *channelMenu = ChannelPopUpMenu(this, feed, channel);
			if (channelMenu->CountItems() == 0) {
				delete channelMenu;
				channelMenu = NULL;
			};
			row->SetMenu(channelMenu);
		};
		
		fChannelList->ExpandOrCollapse(feedRow, true);
	};
	
	delete newItemSpec;
	delete unreadItemSpec;
	delete newItemChanSpec;
	delete unreadItemChanSpec;
};

void TWindow::ClearFeedList(void) {
	fSelectedChannel = NULL;
	
	while (fChannelList->CountRows() > 0) {
		BRow *row = fChannelList->RowAt(0L);
		
		while (fChannelList->CountRows(row) > 0) {
			BRow *child = fChannelList->RowAt(0L, row);
			fChannelList->RemoveRow(child);
			
			delete child;
		};
		
		fChannelList->RemoveRow(row);
		delete row;
	};
	
	fFeedRow.clear();
};

void TWindow::ClearItemList(void) {
	fBox->SetLabel("");
	fSelectedItem = NULL;

	// Clear the item list
	while (fItemList->CountRows() > 0) {
		BRow *row = fItemList->RowAt(0);
		fItemList->RemoveRow(row);
		
		delete row;
	};
};

void TWindow::ClearContentView(void) {
	BFile file(kBlankPath, B_CREATE_FILE | B_READ_WRITE);
	file.Write("<html></html>", strlen("<html></html>"));

	VisitURL(kBlankURL);
	
	while (fContentMenu->CountItems() > 0) {
		delete fContentMenu->RemoveItem(0L);
	};
};

void TWindow::ClearEnclosureList(void) {
	fContentEnclosureBox->SetLabel("");

	// Clear the Enclosure list
	while (fEnclosureList->CountRows() > 0) {
		BRow *row = fEnclosureList->RowAt(0);
		fEnclosureList->RemoveRow(row);
		
		delete row;
	};
};

void TWindow::ClearItemRunner(void) {
	delete fItemReadRunner;
	fItemReadRunner = NULL;
};

void TWindow::UpdateEnclosureStatus(FeedKit::Item *item, FeedKit::Enclosure *enclosure, int32 percentage, entry_ref ref) {
	// Check that this download is for the current item
	if ((fSelectedItem != NULL) && (strcmp(item->UUID(), fSelectedItem->UUID()) == 0)) {
	
		for (int32 i = 0; i < fEnclosureList->CountRows(); i++) {
			row_t *row = reinterpret_cast<row_t *>(fEnclosureList->RowAt(i));
			Enclosure *enc = fCache->GetEnclosure(row->Tag().String());
			
			if (strcmp(enc->UUID(), enclosure->UUID()) == 0) {
				fprintf(stderr, "TWindow::UpdateEnclosureStatus(%p, %p, %i, %s)\n", item, enclosure, percentage, ref.name); 

				Lock();
				
				row->SetTag(enclosure->UUID());
				
				BPath path(&ref);
				const DownloadProgress *progress = enclosure->Progress();
				off_t size = enclosure->Size();
				
				if (progress != NULL) size = progress->Size();

				row->SetField(new BIntegerField(percentage), kEnclosureColProgress);
				row->SetField(new BSizeField(size), kEnclosureColSize);
				row->SetField(new BStringField(path.Path()), kEnclosureColPath);
				row->SetField(new BStringField(EnclosureStateName(enclosure->State())), kEnclosureColState);
				fEnclosureList->UpdateRow(row);
				
				Channel *channel = item->ParentChannel();
				Feed *feed = channel->ParentFeed();
				BPopUpMenu *menu = EnclosurePopUpMenu(this, feed, channel, item, enclosure);
				if (menu->CountItems() == 0) {
					delete menu;
					menu = NULL;
				};
				row->SetMenu(menu);

				Unlock();
			};
		};
	};
};

BBitmap *TWindow::ReadNodeIcon(BString &path, int32 size, bool followSymlink = true) {
	BBitmap *ret = NULL;
	entry_ref ref;
	
	fprintf(stderr, "TWindow::ReadNodeIcon(%s, %i, %s)\n", path.String(), size, followSymlink == true ? "true" : "false");
	
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

void TWindow::VisitURL(const char *url) {
	fprintf(stderr, "TWindow::VisitURL(%s) called\n", url);

	BMessage open(B_NETPOSITIVE_OPEN_URL);
	BMessenger msgr(fNetPositive);
		
	open.AddString("be:url", url);
	msgr.SendMessage(&open);
};
