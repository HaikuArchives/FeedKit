#include "TextItemView.h"

#include <Font.h>
#include <TextControl.h>

#include <algorithm>
#include <stdio.h>

//#pragma mark Namespace import

using namespace FeedKit;
using namespace FeedKit::Settings;

//#pragma mark Constants

//#pragma mark Constructor
TextItemView::TextItemView(const char *name, const char *label, display_type displayType,
	int32 dataType, float width, float divider, BMessage configItem, BMessage settings)
	: ConfigItemView(name, label, displayType, dataType, width, divider, configItem, settings),
	fSettings(settings),
	fTextControl(NULL) {
};

TextItemView::~TextItemView(void) {
	if (fTextControl) fTextControl->RemoveSelf();
	delete fTextControl;
};
	
//#pragma mark ConfigItemView hooks

void TextItemView::Revert(void) {
	fTextControl->SetText(ExtractValue(false).String());
};

void TextItemView::Defaults(void) {
	fTextControl->SetText(ExtractValue(true).String());
};

void TextItemView::Save(BMessage &settings) {
	const char *text = fTextControl->Text();
	const char *name = Name();
	
	switch (DataType()) {
		case B_INT32_TYPE: {
			int32 value = atol(text);
			settings.AddInt32(name, value);
		} break;
		
		case B_STRING_TYPE: {
			settings.AddString(name, text);
		} break;
		
	};
	
	settings.PrintToStream();
	fSettings = settings;
};

//#pragma mark BView Hooks

void TextItemView::AttachedToWindow(void) {
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
 	SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
 	SetHighColor(0, 0, 0, 255);

	BRect location(Bounds());
	location.left += Divider();
	location.right = Width();

	fTextControl = new BTextControl(location, Name(), NULL, "", NULL,
		B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE | B_FRAME_EVENTS);
	AddChild(fTextControl);
	fTextControl->ResizeToPreferred();

	fTextControl->TextView()->HideTyping(DisplayType() == Settings::TextPassword);
	fTextControl->SetText(ExtractValue(false).String());
};

void TextItemView::GetPreferredSize(float *width, float *height) {
	font_height fh;
	be_plain_font->GetHeight(&fh);
	
	float w, h;
	fTextControl->GetPreferredSize(&w, &h);
	
	*width = Width();
	*height = ceil(max_c(fh.ascent + fh.leading + fh.descent, h));
};

//#pragma mark Private

BString TextItemView::ExtractValue(bool useDefault) {
	BString value = "";
	switch (DataType()) {
		case B_INT32_TYPE: {
			vec_int32_t values;
			if (useDefault) {
				values = SettingsDefaultInt32();
			} else {
				values = SettingsInt32();
			};
			
			if (values.empty() == false) value << values[0];
		} break;
		
		case B_STRING_TYPE: {
			vec_str_t values;
			if (useDefault) {
				values = SettingsDefaultString();
			} else {
				values = SettingsString();
			};

			if (values.empty() == false) value = values[0];
		} break;
	
		default: {
			value = "<unsupported datatype>";
		} break;
	};
	
	return value;
};

