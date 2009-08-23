#include "FilePickerView.h"

#include <Button.h>
#include <FilePanel.h>
#include <FindDirectory.h>
#include <Font.h>
#include <ListView.h>
#include <NodeInfo.h>
#include <ScrollView.h>
#include <SymLink.h>

#include <algorithm>
#include <map>

#include "Common/IconTextItem.h"
#include "Common/IMKitUtilities.h"

#include <stdio.h>
#ifdef __HAIKU__
#	include <posix/compat/sys/stat.h>
#endif

//#pragma mark Namespace import

using namespace FeedKit;
using namespace FeedKit::Settings;

//#pragma mark Constants

const int32 kMsgShowPicker = 'shpk';
const int32 kMsgPickerDone = 'pckd';
const int32 kMsgDeleteItem = 'deli';
const int32 kMsgDragListViewUpdated = 'dlvu';
const int32 kMsgDragListViewSelected = 'dlvs';

//#pragma mark DirFilter

class DirFilter : public BRefFilter {
	public:
		DirFilter(void) {
		};
		
#ifdef __HAIKU__
		bool Filter(const entry_ref *ref, BNode *node, struct stat_beos *st, const char *mime) {
#else
		bool Filter(const entry_ref *ref, BNode *node, struct stat *st, const char *mime) {
#endif
			bool result = false;
			if (strcmp(mime, "application/x-vnd.Be-directory") == 0) {
				result = true;
			} else if (strcmp(mime, "application/x-vnd.Be-symlink") == 0) {
				BSymLink link(ref);
				char path[B_PATH_NAME_LENGTH];
				char symMime[B_PATH_NAME_LENGTH];
				
				link.ReadLink(path, sizeof(path));

				entry_ref symRef;
				BNode symNode(path);
				BNodeInfo info(&symNode);
				
				get_ref_for_path(path, &symRef);
				info.GetType(symMime);
				
				result = Filter(&symRef, &symNode, NULL, symMime);
			};
			
			return result;
		};
};

//#pragma mark DragListView

typedef std::map<entry_ref, IconTextItem *> refitem_t;

class DragListView : public BListView {
	public:
		DragListView(BRect frame, const char *name, BRefFilter *filter = NULL,
			int32 maxItems = -1, BMessage *itemMsg = NULL,
			list_view_type type = B_SINGLE_SELECTION_LIST,
			uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP,
			uint32 flags = B_WILL_DRAW | B_NAVIGABLE | B_FRAME_EVENTS)
			: BListView(frame, name, type, resizingMode, flags),
			fFilter(filter),
			fMaxItems(maxItems),
			fItemMsg(itemMsg) {
		};
							
		~DragListView(void) {
			refitem_t::iterator rIt;
			for (rIt = fItems.begin(); rIt != fItems.end(); rIt++) delete rIt->second;
			
			delete fItemMsg;
		};

		void AddItem(entry_ref ref) {
			refitem_t::iterator rIt = fItems.find(ref);
			if (rIt == fItems.end()) {
				BPath p(&ref);
				IconTextItem *item = new IconTextItem(p.Path(), ReadNodeIcon(p.Path()), true);
				
				fItems[ref] = item;
				BListView::AddItem(item);
				
				Invoke(new BMessage(*fItemMsg));
			};
		};
		
		bool RemoveItem(entry_ref ref) {
			bool found = false;
			refitem_t::iterator rIt = fItems.find(ref);
			if (rIt != fItems.end()) {
				BListView::RemoveItem(rIt->second);
				delete rIt->second;
				
				fItems.erase(rIt);
				found = true;

				Invoke(new BMessage(*fItemMsg));
			};
			
			return found;
		};

		bool RemoveItem(BListItem *item) {
			bool found = false;
			refitem_t::iterator rIt;
			for (rIt = fItems.begin(); rIt != fItems.end(); rIt++) {
				if (rIt->second == item) {
					BListView::RemoveItem(item);
					delete item;
					
					fItems.erase(rIt);
					found = true;
					
					Invoke(new BMessage(*fItemMsg));
					
					break;
				};
			};
			
			return found;
		};
		
		void MakeEmpty(void) {
			BListView::MakeEmpty();
			
			refitem_t::iterator rIt;
			for (rIt = fItems.begin(); rIt != fItems.end(); rIt++) delete rIt->second;
			
			fItems.clear();
		};

		entry_ref RefAt(int32 index) {
			entry_ref ref;
			BListItem *item = ItemAt(index);
			refitem_t::iterator rIt;
			for (rIt = fItems.begin(); rIt != fItems.end(); rIt++) {
				if (rIt->second == item) {
					ref = rIt->first;
					break;
				};
			};
			
			return ref;
		};

		// BListView Hooks
		virtual bool InitiateDrag(BPoint point, int32 index, bool wasSelected) {
			if (wasSelected = false) return false;
			
			int32 selected = 0;
			int32 i = 0;
			BRect dragRect(0, 0, Bounds().Width() - B_V_SCROLL_BAR_WIDTH, 0);
		
			while ((selected = CurrentSelection(i)) >= 0) {
				BListItem *item = ItemAt(selected);
				dragRect.bottom += item->Height();
				
				i++;
			};

			float offset = 0;
			BMessage dragMsg(B_SIMPLE_DATA);
			BBitmap *image = new BBitmap(dragRect, B_RGBA32, true);
			BView *view = new BView(dragRect, NULL, B_FOLLOW_NONE, B_WILL_DRAW);
		
			image->AddChild(view);
			image->Lock();
		
			selected = 0;
			i = 0;
			
			dragMsg.AddInt32("be:actions", B_TRASH_TARGET);
		
			while ((selected = CurrentSelection(i)) >= 0) {
				BListItem *item = ItemAt(selected);
				entry_ref ref = RefAt(selected);
				BRect itemBounds(0, offset, view->Bounds().Width(), offset + item->Height());
		
				view->ResizeBy(0, item->Height());
				item->DrawItem(view, itemBounds, true);
				dragMsg.AddRef("refs", &ref);
						
				offset += item->Height();
				i++;
			};
			
			view->Sync();
			image->Unlock();
			
			DeselectAll();	
			DragMessage(&dragMsg, image, B_OP_ALPHA, BPoint(10.0, 10.0));
		
			return true;			
		};

		virtual void MessageReceived(BMessage *msg) {
			switch (msg->what) {
				case B_SIMPLE_DATA: {
					entry_ref ref;
					bool itemsAdded = false;
					bool add = false;
					
					int32 items = CountItems();
					if (AllowMoreItems() == false) return;
					
					for (int32 i = 0; msg->FindRef("refs", i, &ref) == B_OK; i++) {
						bool add = true;
						if (fFilter) {
							add = false;
							
							BPath path(&ref);
							BNode node(&ref);
							BNodeInfo info(&node);
							char mime[B_PATH_NAME_LENGTH];

							info.GetType(mime);

							struct stat st;
							stat(path.Path(), &st);

#ifdef __HAIKU__
							struct stat_beos stb;
							convert_to_stat_beos(&st, &stb);
							add = fFilter->Filter((const entry_ref *) &ref, &node, &stb, (const char *) mime);
#else
							add = fFilter->Filter((const entry_ref *) &ref, &node, &st, (const char *) mime);
#endif
						};
						if (add) {
							itemsAdded = true;
							AddItem(ref);
							items++;
							
							if (AllowMoreItems() == false) break;
						};
					};
					
					if (itemsAdded) Invoke(new BMessage(*fItemMsg));
				} break;
				
				case B_TRASH_TARGET: {
					BMessage previous;
					if (msg->FindMessage("_previous_", &previous) != B_OK) return;

					entry_ref ref;

					for (int32 i = 0; previous.FindRef("refs", i, &ref) == B_OK; i++) {
						RemoveItem(ref);
					};
				} break;
			};
		};

		virtual void KeyDown(const char *bytes, int32 numBytes) {
			switch (bytes[0]) {
				case B_DELETE: {
					int32 index = CurrentSelection();
					while (index >= 0) {
						RemoveItem(ItemAt(index));
						
						index = CurrentSelection();
					};
				} break;
			
				default: {
					BListView::KeyDown(bytes, numBytes);
				} break;
			};
		};
		
		bool AllowMoreItems(void) {
			bool ret = true;
			if ((fMaxItems > -1) && (CountItems() >= fMaxItems)) ret = false;

			return ret;
		};
	
	private:	
		refitem_t		fItems;
		BRefFilter		*fFilter;
		int32			fMaxItems;
		BMessage		*fItemMsg;
};

//#pragma mark Constructor

FilePickerView::FilePickerView(const char *name, const char *label, display_type displayType,
	int32 dataType, float width, float divider, BMessage configItem, BMessage settings)
	: ConfigItemView(name, label, displayType, dataType, width, divider, configItem, settings),
	fSettings(settings),
	fButtonPick(NULL),
	fRefFilter(NULL),
	fListView(NULL),
	fScrollView(NULL) {
	
	fRefFilter = new DirFilter();
};

FilePickerView::~FilePickerView(void) {
	if (fButtonPick) fButtonPick->RemoveSelf();
	delete fButtonPick;
	
	if (fButtonDelete) fButtonDelete->RemoveSelf();
	delete fButtonDelete;
	
	if (fListView) fListView->RemoveSelf();
	delete fListView;
	
	delete fRefFilter;
};
	
//#pragma mark ConfigItemView hooks

void FilePickerView::Revert(void) {
	fListView->MakeEmpty();
	BuildContents(false);
};

void FilePickerView::Defaults(void) {
	fListView->MakeEmpty();
	BuildContents(true);
};

void FilePickerView::Save(BMessage &settings) {
	int32 items = fListView->CountItems();
	const char *name = Name();
	
	for (int32 i = 0; i < items; i++) {
		entry_ref ref = fListView->RefAt(i);
		settings.AddRef(name, &ref);
	};
	
	fSettings = settings;
};

//#pragma mark BView Hooks

void FilePickerView::AttachedToWindow(void) {
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
 	SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
 	SetHighColor(0, 0, 0, 255);
 	
 	BRect location(Bounds());
 	location.left += Divider();
 	location.right = Width() - B_V_SCROLL_BAR_WIDTH;
 	location.bottom = 100;
	location.InsetBy(Padding(), Padding());
	location.left -= Padding() / 2;
		
	int32 type = DisplayType();
	BRefFilter *filter = NULL;
	int32 limit = -1;
	if ((type == Settings::DirectoryPickerSingle) || (type == Settings::DirectoryPickerMultiple)) {
		filter = fRefFilter;
	};
	if ((type == Settings::DirectoryPickerSingle) || (type == Settings::FilePickerSingle)) {
		limit = 1;
	};
 	
 	fListView = new DragListView(location, Name(), filter, limit,
 		new BMessage(kMsgDragListViewUpdated), B_MULTIPLE_SELECTION_LIST,
 		B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	
	fScrollView = new BScrollView("ScrollView", fListView, B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP,
		B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE, false, true);
	AddChild(fScrollView);
	
	fListView->SetTarget(this);
	fListView->SetSelectionMessage(new BMessage(kMsgDragListViewSelected));

	location.top = location.bottom + (Padding() * 2);
	location.bottom = location.top + 100;

	// Add button	
	fButtonPick = new BButton(location, "PickFiles", "Add", new BMessage(kMsgShowPicker),
		B_FOLLOW_RIGHT, B_WILL_DRAW | B_NAVIGABLE);
	AddChild(fButtonPick);
	
	fButtonPick->ResizeToPreferred();
	fButtonPick->MoveTo(Bounds().Width() - fButtonPick->Bounds().Width(), location.top);
	fButtonPick->SetTarget(this);

	// Delete button	
	fButtonDelete = new BButton(location, "DeleteFile", "Remove", new BMessage(kMsgDeleteItem),
		B_FOLLOW_RIGHT, B_WILL_DRAW | B_NAVIGABLE);
	AddChild(fButtonDelete);
	
	fButtonDelete->ResizeToPreferred();
	fButtonDelete->MoveTo(fButtonPick->Frame().left - Padding() - fButtonDelete->Bounds().Width(),
		location.top);
	fButtonDelete->SetTarget(this);
	fButtonDelete->SetEnabled(false);

	BuildContents(false);
};

void FilePickerView::GetPreferredSize(float *width, float *height) {
	font_height fh;
	be_plain_font->GetHeight(&fh);
	
	*width = Width();
//	*height = fButtonPick->Frame().bottom - fScrollView->Frame().top;
//	*height = fButtonPick->Frame().bottom - fScrollView->Frame().top;
	*height = fButtonPick->Frame().bottom + Padding();// * 2; // XXX
	
//	printf("FilePickerView:\nFrame:"); Frame().PrintToStream();
//	printf("Bounds:"); Bounds().PrintToStream();
//	printf("ButtonPick:\nFrame:"); fButtonPick->Frame().PrintToStream();
//	printf("Bounds:"); fButtonPick->Bounds().PrintToStream();
//	printf("ScrollView:\nFrame:"); fScrollView->Frame().PrintToStream();
//	printf("Bounds:"); fScrollView->Bounds().PrintToStream();
};

void FilePickerView::MessageReceived(BMessage *msg) {
	switch (msg->what) {
		case kMsgShowPicker: {
			entry_ref startRef;
			if (fListView->CountItems() > 0) {
				startRef = fListView->RefAt(0);
			} else {
				BPath path;
				find_directory(B_USER_DIRECTORY, &path);
				get_ref_for_path(path.Path(), &startRef);
			};

			int displayType = DisplayType();
			int32 flavours  = 0;
			bool multiple = false;
			BRefFilter *filter = NULL;
			if ((displayType == Settings::DirectoryPickerSingle) ||
				(displayType == Settings::DirectoryPickerMultiple)) {

				filter = fRefFilter;
				flavours = B_DIRECTORY_NODE;
			} else {
				flavours = B_FILE_NODE;
			};
			
			if ((displayType == Settings::DirectoryPickerMultiple) ||
				(displayType == Settings::FilePickerMulti)) {
				multiple = true;
			};
			
			BFilePanel *panel = new BFilePanel(B_OPEN_PANEL, new BMessenger(this), &startRef,
				flavours, multiple, new BMessage(kMsgPickerDone), filter, false, true);
			panel->SetTarget(this);
			panel->Show();
		} break;
		
		case kMsgPickerDone: {	
			entry_ref ref;
			for (int32 i = 0; msg->FindRef("refs", i, &ref) == B_OK; i++) fListView->AddItem(ref);
		} break;
		
		case kMsgDragListViewUpdated: {
			fButtonPick->SetEnabled(fListView->AllowMoreItems());
		} break;
		
		case kMsgDragListViewSelected: {
			fButtonDelete->SetEnabled(fListView->CurrentSelection() >= 0);
		} break;

		case kMsgDeleteItem: {
			int32 index = fListView->CurrentSelection();
			
			while (index >= 0) {
				BListItem *item = fListView->ItemAt(index);
				fListView->RemoveItem(item);
				
				index = fListView->CurrentSelection();
			};
		} break;


		default: {
			ConfigItemView::MessageReceived(msg);
		};
	};
};

//#pragma mark Private

void FilePickerView::BuildContents(bool useDefault) {
	vec_ref_t refs;

	if (useDefault) {
		refs = SettingsDefaultRef();
	} else {
		refs = SettingsRef();
	};

	int32 refCount = refs.size();
	for (int32 i = 0; i < refCount; i++) fListView->AddItem(refs[i]);
};
