#include "MultiTextItemView.h"

#include <Font.h>
#include <TextView.h>
#include <ScrollView.h>

#include <algorithm>

//#pragma mark Namespace import

using namespace FeedKit;
using namespace FeedKit::Settings;

//#pragma mark Constants

const int32 kLineCount = 3;

//#pragma mark Constructor
MultiTextItemView::MultiTextItemView(const char *name, const char *label, display_type displayType,
	int32 dataType, float width, float divider, BMessage configItem, BMessage settings)
	: ConfigItemView(name, label, displayType, dataType, width, divider, configItem, settings),
	fSettings(settings),
	fTextView(NULL) {
};

MultiTextItemView::~MultiTextItemView(void) {
	if (fScrollView) fScrollView->RemoveSelf();
	delete fScrollView;
};
	
//#pragma mark ConfigItemView hooks

void MultiTextItemView::Revert(void) {
	fTextView->SetText(ExtractValue(false).String());
};

void MultiTextItemView::Defaults(void) {
	fTextView->SetText(ExtractValue(true).String());
};

void MultiTextItemView::Save(BMessage &settings) {
	const char *text = fTextView->Text();
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
	fSettings = settings;
};

//#pragma mark BView Hooks

void MultiTextItemView::AttachedToWindow(void) {
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
 	SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
 	SetHighColor(0, 0, 0, 255);

	BRect location(Bounds());
	location.left += Divider();
	location.right = Width() - B_V_SCROLL_BAR_WIDTH;
	location.InsetBy(Padding(), Padding());
	location.left -= Padding() / 2;

	BRect textRect(location);
	textRect.OffsetTo(B_ORIGIN);
	textRect.InsetBySelf(Padding(), Padding());
	
	fTextView = new BTextView(location, Name(), textRect, B_FOLLOW_ALL_SIDES,
		B_WILL_DRAW | B_NAVIGABLE);

	fScrollView = new BScrollView("ScrollView", fTextView, B_FOLLOW_ALL_SIDES,
		B_WILL_DRAW | B_NAVIGABLE, false, true);
	AddChild(fScrollView);
	
	fTextView->SetText(ExtractValue(false).String());
};

void MultiTextItemView::GetPreferredSize(float *width, float *height) {
	font_height fh;
	be_plain_font->GetHeight(&fh);
	
	*width = Width();
	*height = ceil(fh.ascent + fh.leading + fh.descent) * kLineCount + (Padding() * 3);
};

//#pragma mark Private

BString MultiTextItemView::ExtractValue(bool useDefault) {
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

