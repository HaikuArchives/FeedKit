#include "PApplication.h"

//#pragma mark Constructor

PApplication::PApplication(void)
	: BApplication("application/x-vnd.beclan-FeedKit-Prefs") {
};

PApplication::~PApplication(void) {
};

//#pragma mark BApplicaiton Hooks

void PApplication::ReadyToRun(void) {
	fWindow = new PWindow();
};

bool PApplication::QuitRequested(void) {
	return true;
};

void PApplication::MessageReceived(BMessage *msg) {
	BApplication::MessageReceived(msg);
};

