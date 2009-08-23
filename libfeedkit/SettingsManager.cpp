#include "SettingsManager.h"

#include <libfeedkit/FeedKitConstants.h>

#include <Application.h>
#include <Directory.h>
#include <File.h>
#include <FindDirectory.h>
#include <Roster.h>

#include <stdio.h>
#include <string.h>

#include "Common/IMKitUtilities.h"

//#pragma mark Namespace import

using namespace FeedKit;
using namespace FeedKit::Settings;
using namespace FeedKit::Settings::AppTypes;

//#pragma mark Constants

const char *kSettingsDir = "Settings";
const char *kTemplateDir = "Templates";
const char *kDisplayAttr = "feedkit:displayname";

namespace FeedKit {
	namespace Settings {
		namespace AppTypes {
			const char *SettingClient = "Client";
			const char *SettingServer = "Server";
			const char *SettingUtil = "Utility";
			const char *SettingParser = "Parser";
			const char *SettingFeed = "Feed";
		};
	};
};

//#pragma mark Functions

void CopyDefaultSettings(BMessage item, const char *name, int32 type, BMessage *settings) {
	switch (type) {
		case B_INT32_TYPE: {
			int32 value = B_ERROR;
			
			for (int32 i = 0; item.FindInt32("default_value", i, &value) == B_OK; i++) {
				settings->AddInt32(name, value);
			}; 
		} break;

		case B_STRING_TYPE: {
			const char *value = NULL;
			
			for (int32 i = 0; item.FindString("default_value", i, &value) == B_OK; i++) {
				settings->AddString(name, value);
			}; 
		} break;
		
		case B_REF_TYPE: {
			entry_ref value;
			
			for (int32 i = 0; item.FindRef("default_value", i, &value) == B_OK; i++) {
				settings->AddRef(name, &value);
			}; 
		} break;
		
	};
};

//#pragma mark Constructor

SettingsManager::SettingsManager(const char *type, const char *app)
	: BInvoker(),
	fType(type),
	fApp(app),
	fDisplayName(app) {
	
	find_directory(B_USER_SETTINGS_DIRECTORY, &fSettingsPath, true);

	// Ensure the directories exist
	BDirectory dir(fSettingsPath.Path());
	dir.CreateDirectory("BeClan", NULL);
	dir.CreateDirectory("BeClan/FeedKit", NULL);

	fSettingsPath.Append("BeClan/FeedKit/");
	BPath p(fSettingsPath);
	p.Append(fType.String());
	dir.CreateDirectory(p.Path(), NULL);

	dir.SetTo(p.Path());
	dir.CreateDirectory(kTemplateDir, NULL);
	dir.CreateDirectory(kSettingsDir, NULL);
	
	BPath templatePath(fSettingsPath);
	templatePath.Append(fType.String());
	templatePath.Append(kTemplateDir);
	templatePath.Append(fApp.String());

	BEntry entry(templatePath.Path());
	if (entry.Exists() == true) {
		int32 length = B_ERROR;
		char *override = ReadAttribute(BNode(templatePath.Path()), kDisplayAttr, &length);
		if ((length > 0) && (override != NULL)) fDisplayName.SetTo(override, length);
		
		free(override);
	};
};

SettingsManager::~SettingsManager(void) {
};

//#pragma mark Public

const char *SettingsManager::Type(void) {
	return fType.String();
};

const char *SettingsManager::App(void) {
	return fApp.String();
};

status_t SettingsManager::DisplayName(const char *name) {
	status_t result = B_ERROR;
	if (name == NULL) name = fApp.String();
	fDisplayName = name;
	
	BPath templatePath(fSettingsPath);
	templatePath.Append(fType.String());
	templatePath.Append(kTemplateDir);
	templatePath.Append(fApp.String());
	
	BEntry entry(templatePath.Path());
	if (entry.Exists() == false) BFile file(templatePath.Path(), B_CREATE_FILE);

	int32 length = B_ERROR;
	result = WriteAttribute(BNode(templatePath.Path()), kDisplayAttr, name, strlen(name), B_STRING_TYPE);
	
	return result;
};

const char *SettingsManager::DisplayName(void) {
	return fDisplayName.String();
};

BMessage SettingsManager::Settings(void) {
	BMessage settings;
	BPath path(fSettingsPath);
	path.Append(fType.String());
	path.Append(kSettingsDir);
	path.Append(fApp.String());
	
	BFile settingsFile(path.Path(), B_READ_WRITE);
	if (settingsFile.InitCheck() != B_OK) {
		BMessage tmplate = Template();
		BMessage item;
		
		for (int32 i = 0; tmplate.FindMessage("setting", i, &item) == B_OK; i++) {
			int32 type = B_ERROR;
			const char *name = NULL;
			
			if (item.FindInt32("type", &type) != B_OK) continue;
			if (item.FindString("name", &name) != B_OK) continue;
			
			CopyDefaultSettings(item, name, type, &settings);
		};
		
		settingsFile.SetTo(path.Path(), B_READ_WRITE | B_CREATE_FILE);
		settings.Flatten(&settingsFile, NULL);
	} else {
		bool modified = false;
		BMessage tmplate = Template();
		BMessage item;

		settings.Unflatten(&settingsFile);
		
		for (int32 i = 0; tmplate.FindMessage("setting", i, &item) == B_OK; i++) {
			int32 type = B_ERROR;
			const char *name = NULL;
			
			if (item.FindInt32("type", &type) != B_OK) continue;
			if (item.FindString("name", &name) != B_OK) continue;
			
			if (settings.GetInfo(name, NULL) == B_NAME_NOT_FOUND) {
				CopyDefaultSettings(item, name, type, &settings);
				modified = true;
			};
		};
		
		// If the settings have been changed re-save them
		if (modified == true) {
			settings.Flatten(&settingsFile, NULL);
		};
	};
	
	return settings;
};

status_t SettingsManager::Settings(BMessage *settings) {
	BPath path(fSettingsPath);
	path.Append(fType.String());
	path.Append(kSettingsDir);
	path.Append(fApp.String());
	
	BFile settingsFile(path.Path(), B_READ_WRITE | B_CREATE_FILE);
	status_t result = settingsFile.InitCheck();
		
	if (result == B_OK) result = settings->Flatten(&settingsFile);

	return result;
};

BMessage SettingsManager::Template(void) {
	BMessage tmplate;
	BPath path(fSettingsPath);
	path.Append(fType.String());
	path.Append(kTemplateDir);
	path.Append(fApp.String());
	
	BFile templateFile(path.Path(), B_READ_WRITE);
	status_t result = templateFile.InitCheck();
	if (result == B_OK) {
		result = tmplate.Unflatten(&templateFile);
		if (result != B_OK) {
			fprintf(stderr, "SettingsManager::Template: Unable to read %s/%s's template: %s (%i)\n",
				fType.String(), fApp.String(), strerror(result), result);
		};
	} else {
		fprintf(stderr, "SettingsManager::Template: Unable to open %s/%s's template: %s (%i)\n",
			fType.String(), fApp.String(), strerror(result), result);
	};

	return tmplate;
};

status_t SettingsManager::Template(BMessage *tmplate, const char *appsig) {
	BPath path(fSettingsPath);
	path.Append(fType.String());
	path.Append(kTemplateDir);
	path.Append(fApp.String());
	
	BFile templateFile(path.Path(), B_READ_WRITE | B_CREATE_FILE);
	status_t result = templateFile.InitCheck();

	if (appsig != NULL) {
		const char *sig = NULL;
		if (tmplate->FindString("app_sig", &sig) != B_OK) {
			tmplate->AddString("app_sig", appsig);
		} else {
			tmplate->ReplaceString("app_sig", appsig);
		};
	};
	
	if (result == B_OK) result = tmplate->Flatten(&templateFile);

	return result;
};

status_t SettingsManager::WatchSettings(uint32 flags) {
	status_t result = B_ENTRY_NOT_FOUND;
	BPath path(fSettingsPath);
	path.Append(fType.String());
	path.Append(kSettingsDir);
	path.Append(fApp.String());
	
	BEntry entry(path.Path());

	if (entry.Exists()) {
		node_ref nref;
		result = entry.GetNodeRef(&nref);

		if (result == B_OK) result = watch_node(&nref, flags, Messenger());
	};
	
	return result;
};

BBitmap *SettingsManager::Icon(int32 size) {
	return new BBitmap(BRect(0, 0, size - 1, size - 1), B_RGBA32);
};

status_t SettingsManager::Icon(entry_ref source, Icon::Location location) {
	return B_ERROR;
};
