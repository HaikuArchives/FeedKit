#include "PWindow.h"

#include <Box.h>
#include <Button.h>
#include <CheckBox.h>
#include <Directory.h>
#include <Entry.h>
#include <FindDirectory.h>
#include <Menu.h>
#include <MenuItem.h>
#include <MenuField.h>
#include <OutlineListView.h>
#include <Path.h>
#include <PopUpMenu.h>
#include <ScrollView.h>
#include <StringView.h>
#include <Roster.h>
#include <TextControl.h>
#include <TextView.h>
#include <View.h>


#ifdef ZETA
#include <locale/Locale.h>
#else
#define _T(str) (str)
#endif

#include "IconTextItem.h"
#include "IMKitUtilities.h"
#include "ResizeView.h"

#include <libfeedkit/BubbleHelper.h>
#include <libfeedkit/SettingsView.h>
#include <libfeedkit/SettingsManager.h>

//#pragma mark Support Classes

class ViewInfo {
	public:
		ViewInfo(SettingsManager *manager, SettingsView *view)
			: Manager(manager),
			View(view) {
		};

		~ViewInfo(void) {
			delete Manager;

			if (View) View->RemoveSelf();
			delete View;
		};
	
		SettingsManager *Manager;
		SettingsView *View;
};

//#pragma mark Constants

const float kControlOffset = 5.0;
const float kEdgeOffset = 5.0;
const float kDividerWidth = 100;

const int32 kMsgSave = 'save';
const int32 kMsgRevert = 'rvrt';
const int32 kMsgDefault = 'dflt';
const int32 kMsgListChanged = 'lsch';

//#pragma mark Consturctor

PWindow::PWindow(void)
	: BWindow(BRect(25, 25, 460, 285), "Feed Kit", B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS),
	 fView(NULL),
	 fSave(NULL),
	 fRevert(NULL),
	 fDefault(NULL),
	 fListView(NULL),
	 fBox(NULL),
	 fCurrentView(NULL) {

	fHelper = new BubbleHelper();
	
	BRect frame = Bounds();

	fView = new BView(frame, "PrefView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW);

	AddChild(fView);
#if B_BEOS_VERSION > B_BEOS_VERSION_5
	fView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	fView->SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	fView->SetHighColor(ui_color(B_PANEL_TEXT_COLOR));
#else
	fView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	fView->SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	fView->SetHighColor(0, 0, 0, 0);
#endif

	frame.left = kEdgeOffset;
	frame.top = kEdgeOffset;
	frame.bottom = Bounds().bottom - kEdgeOffset;
	frame.right = 100;
	fListView = new BOutlineListView(frame, "LISTVIEW", B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);

	font_height fontHeight;
	be_bold_font->GetHeight(&fontHeight);
	fFontHeight = fontHeight.descent + fontHeight.leading + fontHeight.ascent;

	fBox = new BBox(BRect(fListView->Bounds().right + (kEdgeOffset * 3) + (kEdgeOffset * 3) +
		B_V_SCROLL_BAR_WIDTH, kEdgeOffset, 	fView->Bounds().right - kEdgeOffset,
		fView->Bounds().bottom - kEdgeOffset), "BOX",
		B_FOLLOW_ALL_SIDES);
	fBox->SetLabel("General");
	
	BScrollView *scroller = new BScrollView("list scroller", fListView, B_FOLLOW_LEFT |
		B_FOLLOW_TOP_BOTTOM, 0, false, true);
	fListView->Select(0);
	fListView->MakeFocus();

	ResizeView *rvLeftRight = new ResizeView(scroller, fBox, fView->Bounds(), "LeftRightResize", B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	fView->AddChild(rvLeftRight);
	rvLeftRight->AddChild(scroller);
	rvLeftRight->AddChild(fBox);

	frame = fBox->Bounds();
	frame.InsetBy(kEdgeOffset, kEdgeOffset);
	frame.bottom -= (kEdgeOffset * 3);
	frame.top = frame.bottom - ((fontHeight.descent + fontHeight.leading + fontHeight.ascent));
	frame.left = frame.right - (be_plain_font->StringWidth(_T("Save")) +
		(kControlOffset * 2));
	float w, h = -1;

	fSave = new BButton(frame, "Save", _T("Save"), new BMessage(kMsgSave),
		B_FOLLOW_BOTTOM | B_FOLLOW_RIGHT);
	fBox->AddChild(fSave);
	fSave->GetPreferredSize(&w, &h);
	fSave->ResizeToPreferred();
	fSave->MoveTo(frame.right - w, fSave->Frame().bottom - h);

	frame.right = fSave->Frame().left - kControlOffset;
	frame.left = frame.right - (be_plain_font->StringWidth(_T("Revert")) +
		(kControlOffset * 2));

	fRevert = new BButton(frame, "Revert", _T("Revert"), new BMessage(kMsgRevert),
		B_FOLLOW_BOTTOM | B_FOLLOW_RIGHT);
	fBox->AddChild(fRevert);
	fRevert->GetPreferredSize(&w, &h);
	fRevert->ResizeToPreferred();
	fRevert->MoveTo(frame.right - w, fRevert->Frame().bottom - h);

	frame.right = fRevert->Frame().left - kControlOffset;
	frame.left = 0;
	
	fDefault = new BButton(frame, "Default", _T("Defaults"), new BMessage(kMsgDefault),
		B_FOLLOW_BOTTOM | B_FOLLOW_RIGHT);
	fBox->AddChild(fDefault);
	fDefault->GetPreferredSize(&w, &h);
	fDefault->ResizeToPreferred();
	fDefault->MoveTo(frame.right - w, fDefault->Frame().bottom - h);

	fListView->SetSelectionMessage(new BMessage(kMsgListChanged));
	fListView->SetTarget(this);

	frame = fBox->Bounds();
	frame.InsetBy(kEdgeOffset, kEdgeOffset);
	frame.top += fFontHeight;
	
	float maxHeight = 0;
	
	BPath settingsPath;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &settingsPath) == B_OK) {
		fSettingPath = settingsPath;
		fSettingPath.Append("BeClan/FeedKit/");
		
		BDirectory rootDir(fSettingPath.Path());
		entry_ref ref;

		while (rootDir.GetNextRef(&ref) == B_OK) {
			BDirectory dir(&ref);
			if (dir.InitCheck() != B_OK) continue;
			if (dir.CountEntries() == 0) continue;
			if (dir.Contains("Templates", B_DIRECTORY_NODE) == false) continue;

			BDirectory templateDir(&dir, "Templates");
			if (templateDir.InitCheck() != B_OK) continue;
			if (templateDir.CountEntries() == 0) continue;
			
			BPath path(&ref);
			IconTextItem *item = new IconTextItem(ref.name, ReadNodeIcon(path.Path()));
			fListView->AddItem(item);

			float maxH = ExtractSettings(item, frame, &templateDir, ref.name);
			maxHeight = max_c(maxH, maxHeight);
		};
	} else {
		fprintf(stderr, "PWindow(): Unable to determine settings dir");
	};
	
	fListView->Select(0);
	
	pref_t::iterator pIt;
	float width = frame.Width();
	for (pIt = fPrefView.begin(); pIt != fPrefView.end(); pIt++) {
		pIt->second->View->ResizeTo(width, maxHeight);
	};

	float requiredHeight = fDefault->Frame().Height() + maxHeight + (kEdgeOffset * 9) + fFontHeight;
	ResizeTo(Frame().Width(), requiredHeight * 1.1);
	
	fView->Show();

	Show();
};

PWindow::~PWindow(void) {
};

//#pragma mark BWindow Hooks

bool PWindow::QuitRequested(void) {
	pref_t::iterator pIt;
	for (pIt = fPrefView.begin(); pIt != fPrefView.end(); pIt++) delete pIt->second;

	if (fSave) fSave->RemoveSelf();
	delete fSave;
	
	if (fRevert) fRevert->RemoveSelf();
	delete fRevert;
		
	if (fDefault) fDefault->RemoveSelf();
	delete fDefault;
		
	if (fBox) fBox->RemoveSelf();
	delete fBox;
	
	if (fListView) fListView->RemoveSelf();
	delete fListView;
	
	if (fView) fView->RemoveSelf();
	delete fView;
	
	be_app_messenger.SendMessage(B_QUIT_REQUESTED);

	return true;
};

void PWindow::MessageReceived(BMessage *msg) {
	switch (msg->what) {
		case kMsgListChanged: {
			int32 index = B_ERROR;
			if (msg->FindInt32("index", &index) != B_OK) return;
			
			IconTextItem *item = reinterpret_cast<IconTextItem *>(fListView->FullListItemAt(index));
			if (item == NULL) return;
			
			IconTextItem *parent = reinterpret_cast<IconTextItem *>(fListView->Superitem(item));
			if (parent == NULL) return;
			
			BString key = parent->Text();
			key << "/" << item->Text();
			
			pref_t::iterator pIt = fPrefView.find(key);
			if (pIt != fPrefView.end()) {
				if (fCurrentView) fCurrentView->Hide();
				pIt->second->View->Show();
				fCurrentView = pIt->second->View;
				
				fBox->SetLabel(item->Text());
				fView->Invalidate();
			};
		} break;

		case kMsgRevert: {
			if (fCurrentView) fCurrentView->Revert();
		} break;

		case kMsgDefault: {
			if (fCurrentView) fCurrentView->Default();
		} break;
		
		case kMsgSave: {
			int32 index = fListView->FullListCurrentSelection();
			IconTextItem *item = reinterpret_cast<IconTextItem *>(fListView->FullListItemAt(index));
			if (item == NULL) return;
			
			IconTextItem *parent = reinterpret_cast<IconTextItem *>(fListView->Superitem(item));
			if (parent == NULL) return;
			
			BString key = parent->Text();
			key << "/" << item->Text();
			
			pref_t::iterator pIt = fPrefView.find(key);
			if (pIt == fPrefView.end()) return;
			
			SettingsView *current = pIt->second->View;
			if (current == NULL) return;

			BMessage settings;
			current->Save(settings);

			pIt->second->Manager->Settings(&settings);
		} break;

		default: {
			BWindow::MessageReceived(msg);
		};
	};
};

//#pragma mark Public

//#pragma mark Private

float PWindow::ExtractSettings(IconTextItem *parent, BRect frame, BDirectory *templateDir,
	const char *apptype) {

	float maxHeight = 0;
	entry_ref templateRef;
		
	while (templateDir->GetNextRef(&templateRef) == B_OK) {
		SettingsManager *manager = new SettingsManager(apptype, templateRef.name);
		BMessage settings = manager->Settings();
		BMessage tmplate = manager->Template();
		BBitmap *icon = manager->Icon(kSmallIcon);
		const char *name = manager->DisplayName();
		
		IconTextItem *item = new IconTextItem(name, icon);
		fListView->AddUnder(item, parent);
		
		SettingsView *view = new SettingsView(frame, apptype, templateRef.name, B_FOLLOW_ALL_SIDES,
			B_WILL_DRAW, tmplate, settings, fHelper);
		
		BString key = parent->Text();
		key << "/" << name;

		ViewInfo *info = new ViewInfo(manager, view);
		fPrefView[key] = info;
		
		fBox->AddChild(view);
		view->Hide();

		view->ResizeToPreferred();
		maxHeight = max_c(maxHeight, view->Frame().Height());
	};
	
	return maxHeight;
};
