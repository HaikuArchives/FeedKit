#include "ClientTemplate.h"

#include <libfeedkit/FeedKitConstants.h>
#include <libfeedkit/FeedListener.h>
#include <libfeedkit/Channel.h>
#include <libfeedkit/Item.h>

//#pragma mark Constants

const char *kAppSignature = "application/x-vnd.beclan.feedkit.ClientTemplate";

// Message whats

//#pragma mark Constructor

ClientTemplate::ClientTemplate(void)
	: BApplication(kAppSignature),
	 fFeed(NULL) {
};

ClientTemplate::~ClientTemplate(void) {
};

//#pragma mark BApplication Hooks
					
void ClientTemplate::MessageReceived(BMessage *msg) {
	BApplication::MessageReceived(msg);
};

void ClientTemplate::ReadyToRun(void) {
	fFeed = new FeedListener();
	fFeed->StartListening();
	fFeed->AddHandler(this);
};

bool ClientTemplate::QuitRequested(void) {
	fFeed->StopListening();
	BMessenger(fFeed).SendMessage(B_QUIT_REQUESTED);
	
	return BApplication::QuitRequested();
};

//#pragma mark Private

