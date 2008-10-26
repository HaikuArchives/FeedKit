#include "SettingsView.h"
#include "FeedKitConstants.h"
#include "BubbleHelper.h"

#include "ConfigItemView.h"
#include "ConfigItemViews/FilePickerView.h"
#include "ConfigItemViews/MenuItemView.h"
#include "ConfigItemViews/MultiTextItemView.h"
#include "ConfigItemViews/TextItemView.h"

#include <libfeedkit/FeedKitConstants.h>

#include <CheckBox.h>
#include <Menu.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <String.h>
#include <TextControl.h>

#include <stdio.h>
#include <algorithm>

//#pragma mark Namespace import

using namespace FeedKit;
using namespace FeedKit::Settings;

//#pragma mark Types

typedef vector<BString> vec_str_t;
typedef vector<int32> vec_int_t;

// The Member Function Pointer for a view maker
typedef ConfigItemView *(*ViewMaker)(const char *, const char *, Settings::display_type, int32,
	float, float, BMessage, BMessage);
// Maps a type to a maker
typedef map<Settings::display_type, ViewMaker> maker_t;

//#pragma mark Global (Yes, bad, I know)

maker_t gMaker;

//#pragma mark C Functions

ConfigItemView *MakeMenuItem(const char *name, const char *label, 
	Settings::display_type dispType, int32 type, float viewWidth, float labelWidth,
	BMessage templateMsg, BMessage settingsMsg) {
	
	return new MenuItemView(name, label, dispType, type, viewWidth, labelWidth, templateMsg,
		settingsMsg);
};

ConfigItemView *MakeTextMulti(const char *name, const char *label,
	Settings::display_type dispType, int32 type, float viewWidth, float labelWidth,
	BMessage templateMsg, BMessage settingsMsg) {
	
	return new MultiTextItemView(name, label, dispType, type, viewWidth, labelWidth, templateMsg,
		settingsMsg);
};

ConfigItemView *MakeTextSingle(const char *name, const char *label,
	Settings::display_type dispType, int32 type, float viewWidth, float labelWidth,
	BMessage templateMsg, BMessage settingsMsg) {
	
	return new TextItemView(name, label, dispType, type, viewWidth, labelWidth, templateMsg,
		settingsMsg);
};

ConfigItemView *MakeFilePicker(const char *name, const char *label,
	Settings::display_type dispType, int32 type, float viewWidth, float labelWidth,
	BMessage templateMsg, BMessage settingsMsg) {
	
	return new FilePickerView(name, label, dispType, type, viewWidth, labelWidth, templateMsg,
		settingsMsg);
};

ConfigItemView *MakeHidden(const char */*name*/, const char */*label*/,
	Settings::display_type /*dispType*/, int32 /*type*/, float /*viewWidth*/, float /*labelWidth*/,
	BMessage /*templateMsg*/, BMessage /*settingsMsg*/) {
	
	return NULL;
};
 

void SetupItemCreationMap(void) {
	if (gMaker.empty() == true) {
		// Set up the creation map
		gMaker[Settings::MenuSingle] = &MakeMenuItem;
		gMaker[Settings::MenuMulti] = &MakeMenuItem;
		gMaker[Settings::TextMulti] = &MakeTextMulti;
		gMaker[Settings::TextSingle] = &MakeTextSingle;
		gMaker[Settings::TextPassword] = &MakeTextSingle;
		gMaker[Settings::DirectoryPickerSingle] = &MakeFilePicker;
		gMaker[Settings::Hidden] = &MakeHidden;
	};
};

//#pragma mark LabelInfo

class SettingInfo {
	public:
		SettingInfo(const char *name, const char *label, float x, float y, BMessage option,
			BView *control)
			: fName(name),
			fLabel(label),
			fPoint(x, y),
			fOption(option),
			fControl(control) {
		};
		
		~SettingInfo(void) {
			fControl->RemoveSelf();
			delete fControl;
		};
		
		const char *Name(void) {
			return fName.String();
		};
		
		const char *Text(void) {
			return fLabel.String();
		};
		
		BPoint Location(void) {
			return fPoint;
		};
		
		BMessage Option(void) {
			return fOption;
		};
		
		BView *Control(void) {
			return fControl;
		};
		
		void LabelArea(float l, float t, float r, float b) {
			fLabelArea.Set(l, t, r, b);
		};
		
		BRect LabelArea(void) {
			return fLabelArea;
		};
		
	private:
		BString			fName;
		BString			fLabel;
		BPoint			fPoint;
		BMessage		fOption;
		BView			*fControl;
		BRect			fLabelArea;
};

//#pragma mark Constants

const float kPadding = 5.0f;

//#pragma mark Constructor

SettingsView::SettingsView(BRect frame, const char *appType, const char *name, uint32 resizing,
	uint32 flags, BMessage tmplate, BMessage settings, BubbleHelper *helper = NULL)
	:BView(frame, name, resizing, flags),
	fAutoStart(NULL),
	fAppType(appType),
	fTemplateMsg(tmplate),
	fSettingMsg(settings),
	fHelper(helper) {
		
	fSettings.clear();
	
	SetupItemCreationMap();
};

SettingsView::~SettingsView(void) {
	if (fAutoStart) fAutoStart->RemoveSelf();
	delete fAutoStart;

	int32 settings = fSettings.size();
	for (int32 i = 0; i < settings; i++) delete fSettings[i];
};

//#pragma mark BView Hooks

void SettingsView::AttachedToWindow(void) {
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
 	SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
 	SetHighColor(0, 0, 0, 255);

	BuildGUI(fTemplateMsg, fSettingMsg);
};

void SettingsView::MessageReceived(BMessage *msg) {
	switch (msg->what) {
		default: {
			BView::MessageReceived(msg);
		};
	};
};

void SettingsView::GetPreferredSize(float *width, float *height) {
	*width = Frame().Width();

	float totalHeight = 0;
	int32 views = fSettings.size();
	float w = 0;
	float h = 0;

	if (fAutoStart) {
		fAutoStart->GetPreferredSize(&w, &h);
		
		totalHeight += h + kPadding;
	};
	
	for (int32 i = 0; i < views; i++) {
		ConfigItemView *view = fSettings[i];
		view->GetPreferredSize(&w, &h);
		
		totalHeight += h + view->Padding();
	};
	
	printf("%s's height is %.2f\n", Name(), totalHeight);
	
	*height = totalHeight;
};

//#pragma mark Public

void SettingsView::Save(BMessage &settings) {
	if (fAutoStart) {
		settings.AddBool("autostart", fAutoStart->Value() == B_CONTROL_ON);
	};

	int32 views = fSettings.size();
	for (int32 i = 0; i < views; i++) fSettings[i]->Save(settings);
	for (int32 i = 0; i < views; i++) fSettings[i]->SetSettings(settings);
};

void SettingsView::Revert(void) {
	int32 views = fSettings.size();
	for (int32 i = 0; i < views; i++) fSettings[i]->Revert();
};

void SettingsView::Default(void) {
	if (fAutoStart) {
		fAutoStart->SetValue(B_CONTROL_ON);
	};

	int32 views = fSettings.size();
	for (int32 i = 0; i < views; i++) fSettings[i]->Defaults();
};

//#pragma mark Private

void SettingsView::BuildGUI(BMessage tmplate, BMessage settings) {
	int32 settingCount = fSettings.size();
	for (int32 i = 0; i < settingCount; i++) delete fSettings[i];
	fSettings.clear();

	float viewWidth = Frame().Width();
	float labelWidth = 0;
	float yOffset = kPadding;
	font_height fh;
	float fontHeight = -1;
	BMessage settingMsg;

	be_plain_font->GetHeight(&fh);
	fontHeight = ceil(fh.ascent + fh.leading + fh.descent);
	
	if (fAppType == FeedKit::Settings::AppTypes::SettingClient) {
		float w = 0;
		float h = 0;
		fAutoStart = new BCheckBox(Frame(), "AutoStart", "Start client automatically", NULL);

		AddChild(fAutoStart);

		fAutoStart->MoveTo(0, yOffset);
		fAutoStart->GetPreferredSize(&w, &h);
		fAutoStart->ResizeTo(w, h);
		
		yOffset += kPadding + ceil(h);
		
		bool autostart = false;
		settings.FindBool("autostart", &autostart);
		
		if (autostart) {
			fAutoStart->SetValue(B_CONTROL_ON);
		} else {
			fAutoStart->SetValue(B_CONTROL_OFF);
		};
	};
	
	for (int32 i = 0; tmplate.FindMessage("setting", i, &settingMsg) == B_OK; i++) {
		const char *label = NULL;
		if (settingMsg.FindString("label", &label) != B_OK) {
			if (settingMsg.FindString("name", &label) != B_OK) continue;
		};
		
		float width = be_plain_font->StringWidth(label);
		labelWidth = max_c(labelWidth, width);
	};
	
	labelWidth += kPadding + kPadding;
		
	for (int32 i = 0; tmplate.FindMessage("setting", i, &settingMsg) == B_OK; i++) {
		int32 type = B_ERROR;
		const char *name = NULL;
		const char *label = NULL;
		const char *help = NULL;
		Settings::display_type displayType = Settings::Unset;

		// Type and name are compulsory
		if (settingMsg.FindInt32("type", &type) != B_OK) continue;
		if (settingMsg.FindString("name", &name) != B_OK) continue;
		
		if (settingMsg.FindString("label", &label) != B_OK) label = name;
		if (settingMsg.FindString("help", &help) != B_OK) help = NULL;
		if (settingMsg.FindInt32("display_type", (int32 *)&displayType) != B_OK) {
			displayType = Settings::Unset;
		};

		ConfigItemView *view = NULL;
		maker_t::iterator mIt = gMaker.find(displayType);
		if (mIt != gMaker.end()) {
			ViewMaker maker = mIt->second;
			view = maker(name, label, displayType, type, viewWidth, labelWidth,
				settingMsg, fSettingMsg);
		} else {
			fprintf(stderr, "SettingView::AttachedToWindow(): Unhandled displaytype: %4.4s\n",
				&displayType);
		};
		
		fprintf(stderr, "SettingView::AttachedToWindow(): %s's view is %p\n", name, view);

		if (view) {
			AddChild(view);
			view->MoveTo(0, yOffset);
			float w = 0;
			float h = 0;
			
			view->GetPreferredSize(&w, &h);
			view->ResizeTo(w, h);
			
			yOffset += kPadding + ceil(h);

			if (fHelper) fHelper->SetHelp(view, (char *)help);
			
			fSettings.push_back(view);
		};
	};
};
