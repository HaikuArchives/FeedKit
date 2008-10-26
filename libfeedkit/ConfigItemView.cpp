#include "ConfigItemView.h"

#include <Entry.h>
#include <Font.h>

//#pragma mark Namespace import

using namespace FeedKit;
using namespace FeedKit::Settings;

//#pragma mark Constructor

ConfigItemView::ConfigItemView(const char *name, const char *label, display_type displayType,
	int32 dataType, float width, float divider, BMessage configItem, BMessage settings)
	: BView(BRect(0, 0, width, width), name, B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP,
		B_NAVIGABLE | B_WILL_DRAW),
	fName(name),
	fLabel(label),
	fDisplayType(displayType),
	fDataType(dataType),
	fWidth(width),
	fDivider(divider),
	fConfigItem(configItem),
	fSettings(settings) {
	
	font_height fh;
	be_plain_font->GetHeight(&fh);
	fLabelLocation.Set(Divider() - be_plain_font->StringWidth(label) - Padding(),
		fh.ascent + fh.descent + fh.leading);
};

ConfigItemView::~ConfigItemView(void) {
};

//#pragma mark BView Hooks

void ConfigItemView::Draw(BRect /*frame*/) {
	const char *label = Label();
	DrawString(label, fLabelLocation);
};

//#pragma mark General accessor functions

const char *ConfigItemView::Name(void) {
	return fName.String();
};

const char *ConfigItemView::Label(void) {
	return fLabel.String();
};

display_type ConfigItemView::DisplayType(void) {
	return fDisplayType;
};

int32 ConfigItemView::DataType(void) {
	return fDataType;
};

float ConfigItemView::Width(void) {
	return fWidth;
};

float ConfigItemView::Divider(void) {
	return fDivider;
};

BMessage *ConfigItemView::ConfigItem(void) {
	return &fConfigItem;
};

const float ConfigItemView::Padding(void) const {
	return 5.0f;
};

void ConfigItemView::SetSettings(BMessage settings) {
	fSettings = settings;
};

//#pragma mark Data Extraction

vec_str_t ConfigItemView::SettingsString(void) {
	BString value;
	vec_str_t settings;
	const char *name = Name();
	
	for (int32 i = 0; fSettings.FindString(name, i, &value) == B_OK; i++) settings.push_back(value);

	// If there's no settings, fetch the defaults
	if (settings.empty() == true) settings = SettingsDefaultString();
	
	return settings;
};

vec_str_t ConfigItemView::SettingsDefaultString(void) {
	BString value;
	vec_str_t settings;

	for (int32 i = 0; fConfigItem.FindString("default_value", i, &value) == B_OK; i++) {
		settings.push_back(value);
	};
	
	return settings;
};

vec_int32_t ConfigItemView::SettingsInt32(void) {
	int32 value;
	vec_int32_t settings;
	const char *name = Name();
	
	for (int32 i = 0; fSettings.FindInt32(name, i, &value) == B_OK; i++) settings.push_back(value);

	// If there's no settings, fetch the defaults
	if (settings.empty() == true) settings = SettingsDefaultInt32();
	
	return settings;
};

vec_int32_t ConfigItemView::SettingsDefaultInt32(void) {
	int32 value;
	vec_int32_t settings;

	for (int32 i = 0; fConfigItem.FindInt32("default_value", i, &value) == B_OK; i++) {
		settings.push_back(value);
	};
	
	return settings;
};

vec_ref_t ConfigItemView::SettingsRef(void) {
	entry_ref value;
	vec_ref_t settings;
	const char *name = Name();
	
	for (int32 i = 0; fSettings.FindRef(name, i, &value) == B_OK; i++) settings.push_back(value);

	// If there's no settings, fetch the defaults
	if (settings.empty() == true) settings = SettingsDefaultRef();
	
	return settings;
};

vec_ref_t ConfigItemView::SettingsDefaultRef(void) {
	entry_ref value;
	vec_ref_t settings;

	for (int32 i = 0; fConfigItem.FindRef("default_value", i, &value) == B_OK; i++) {
		settings.push_back(value);
	};
	
	return settings;
};
