#include "MenuItemView.h"

#include <Font.h>
#include <Menu.h>
#include <MenuField.h>
#include <MenuItem.h>

#include <algorithm>

//#pragma mark Namespace import

using namespace FeedKit;
using namespace FeedKit::Settings;

//#pragma mark Constants

const int32 kResizeMenu = 'rsze';

//#pragma mark Constructor

MenuItemView::MenuItemView(const char *name, const char *label, display_type displayType,
	int32 dataType, float width, float divider, BMessage configItem, BMessage settings)
	: ConfigItemView(name, label, displayType, dataType, width, divider, configItem, settings),
	fSettings(settings),
	fMenuField(NULL),
	fMenu(NULL) {
};

MenuItemView::~MenuItemView(void) {
	if (fMenuField) fMenuField->RemoveSelf();
	delete fMenuField;
};
	
//#pragma mark ConfigItemView hooks

void MenuItemView::Revert(void) {
	while (fMenu->CountItems() > 0) delete fMenu->RemoveItem(0L);		
	BuildMenu(false);
};

void MenuItemView::Defaults(void) {
	while (fMenu->CountItems() > 0) delete fMenu->RemoveItem(0L);
	BuildMenu(true);
};

void MenuItemView::Save(BMessage &settings) {
	std::vector<BMenuItem *> items;
	BMenuItem *item = NULL;
	int32 type = DataType();
	BMessage *config = ConfigItem();
	const char *name = Name();

	while ((item = fMenu->FindMarked()) != NULL) {
		items.push_back(item);

		item->SetMarked(false);	
		int32 index = fMenu->IndexOf(item);
		
		switch (type) {
			case B_INT32_TYPE: {
				int32 value = B_ERROR;
				if (config->FindInt32("option_value", index, &value) == B_OK) {
					settings.AddInt32(name, value);
				};
			} break;
			
			case B_STRING_TYPE: {
				BString value = "";
				if (config->FindString("option_value", index, &value) == B_OK) {
					settings.AddString(name, value);
				};
			} break;
		};
	};
	
	int32 marked = items.size();
	for (int32 i = 0; i < marked; i++) items[i]->SetMarked(true);
	
	fSettings = settings;
};

//#pragma mark BView Hooks

void MenuItemView::AttachedToWindow(void) {
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
 	SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
 	SetHighColor(0, 0, 0, 255);

	fMenu = new BMenu(Name());	
	BuildMenu(false);
	
	fMenu->SetLabelFromMarked(true);
	fMenu->SetRadioMode(DisplayType() == Settings::MenuSingle);
	
	BRect location(Bounds());
	location.left += Divider();
	location.right = Width();
	fMenuField = new BMenuField(location, Name(), "", fMenu, B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fMenuField->SetDivider(0);
	
	AddChild(fMenuField);
};

void MenuItemView::GetPreferredSize(float *width, float *height) {
	font_height fh;
	be_plain_font->GetHeight(&fh);
	float fontHeight = fh.ascent + fh.leading + fh.descent;
	
	*width = Width();
	*height = ceil(max_c(fontHeight, fMenuField->Frame().bottom));
};

void MenuItemView::MessageReceived(BMessage *msg) {
	switch (msg->what) {
		case kResizeMenu: {
			fMenuField->ResizeToPreferred();
		} break;
	
		default: {
			ConfigItemView::MessageReceived(msg);
		} break;
	};
};

//#pragma mark Private

void MenuItemView::BuildMenu(bool defaultOnly) {
	switch (DataType()) {
		case B_INT32_TYPE: {
			vec_int32_t selected;
			if (defaultOnly) {
				selected = SettingsDefaultInt32();
			} else {
				selected = SettingsInt32();
			};
			bool selectFirst = selected.empty();
			int32 value = B_ERROR;
			BMessage *config = ConfigItem();
			
			for (int32 i = 0; config->FindInt32("option_value", i, &value) == B_OK; i++) {
				const char *label = NULL;
				if (config->FindString("option_label", i, &label) != B_OK) continue;
				
				BMenuItem *item = new BMenuItem(label, new BMessage(kResizeMenu));
				if (selectFirst == true) {
					if (i == 0) item->SetMarked(true);
				} else {
					vec_int32_t::iterator sIt = find(selected.begin(), selected.end(), value);
					if (sIt != selected.end()) item->SetMarked(true);
				};
			
				item->SetTarget(this);
				fMenu->AddItem(item);
			};
			
			fMenu->SetTargetForItems(this);
		} break;
		
		case B_STRING_TYPE: {
			vec_str_t selected;
			if (defaultOnly) {
				selected = SettingsDefaultString();
			} else {
				selected = SettingsString();
			};
			bool selectFirst = selected.empty();
			BString value = "";
			BMessage *config = ConfigItem();
			
			for (int32 i = 0; config->FindString("option_value", i, &value) == B_OK; i++) {
				const char *label = NULL;
				if (config->FindString("option_label", i, &label) != B_OK) continue;
				
				BMenuItem *item = new BMenuItem(label, new BMessage(kResizeMenu));
				if (selectFirst == true) {
					if (i == 0) item->SetMarked(true);
				} else {
					vec_str_t::iterator sIt = find(selected.begin(), selected.end(), value);
					if (sIt != selected.end()) item->SetMarked(true);
				};
				
				item->SetTarget(this);
				fMenu->AddItem(item);
			};
			
			fMenu->SetTargetForItems(this);
		} break;
	};
};
