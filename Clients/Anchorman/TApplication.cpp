#include "TApplication.h"
#include "TWindow.h"
#include "Settings.h"

#include <Archivable.h>
#include <support/ClassInfo.h>

#include <libfeedkit/SettingsManager.h>

//#pragma mark Constants

//#pragma mark Constructor

TApplication::TApplication(bool unarchive = true)
	: BApplication(kAppSignature),
	fUnarchive(unarchive),
	fWindow(NULL) {
};

TApplication::~TApplication(void) {
};

//#pragma mark BApplication Hooks

void TApplication::ReadyToRun(void) {
	FeedKit::Settings::SettingsManager manager(FeedKit::Settings::AppTypes::SettingClient, kSettingsAppName);

	BMessage settings = manager.Settings();
	BMessage archive;

	if ((fUnarchive == true) && (settings.FindMessage(kSettingsMessageName, &archive) == B_OK)) {
		BArchivable *instance = instantiate_object(&archive);
		if (instance) {
			fWindow = cast_as(instance, TWindow);
		};
	};
	
	if (fWindow == NULL) fWindow = new TWindow();

	fWindow->Show();
};

bool TApplication::QuitRequested(void) {
	return true;
};

void TApplication::MessageReceived(BMessage *msg) {
	BApplication::MessageReceived(msg);
};

