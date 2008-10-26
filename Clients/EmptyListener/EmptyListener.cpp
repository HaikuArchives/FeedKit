#include "EmptyListener.h"

#include <libfeedkit/FeedKitConstants.h>
#include <libfeedkit/FeedListener.h>
#include <libfeedkit/Feed.h>
#include <libfeedkit/ChannelInfo.h>
#include <libfeedkit/Channel.h>
#include <libfeedkit/Item.h>

#include <libfeedkit/ServerObjectFactory.h>

//#pragma mark Constants

// Message whats

//#pragma mark Constructor

using namespace FeedKit;

EmptyListener::EmptyListener(void)
	: BApplication("application/x-vnd.beclan.rss.empty_listener"),
	 fRSS(NULL) {
	 
	 ServerObjectFactory *factory = ServerObjectFactory::Instance();
	 printf("Factory: %p\n", factory);
	 
	 Feed *f = factory->GetFeed("wang");
	 printf("Feed: %p\n", f);
	 printf("F: %s (%s)\n", f->Name(), f->URL());
	 
	 exit(0);
	 
};

EmptyListener::~EmptyListener(void) {
	fRSS->StopListening();
	delete fRSS;
};

//#pragma mark BApplication Hooks
					
void EmptyListener::MessageReceived(BMessage *msg) {
//	msg->PrintToStream();

	switch (msg->what) {
		case FeedKit::FromServer::NewItems: {
			Channel c;
			printf("Finding channel: %s\n", strerror(msg->FindFlat("channel", 0, &c)));
			
			printf("Title: %s\n", c.Title());
		} break;
	};

	BApplication::MessageReceived(msg);
};

void EmptyListener::ReadyToRun(void) {
	fRSS = new FeedListener(BMessenger(this));
	fRSS->StartListening();
	
	ChannelInfo *info = new ChannelInfo();
	Channel *c = info->Details();
	
	printf("Channel: %s\n", c->Title());
};

//#pragma mark Private

