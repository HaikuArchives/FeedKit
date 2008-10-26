#ifndef FEEDINFOPOPPER_H
#define FEEDINFOPOPPER_H

#include <Application.h>
#include <Entry.h>

#include <libfeedkit/FeedHandler.h>

class BMessenger;


namespace FeedKit {
	class FeedListener;
	class Channel;
};

using namespace FeedKit;

class FeedInfoPopper : public BApplication, public FeedKit::FeedHandler {
	public:
							FeedInfoPopper(void);
							~FeedInfoPopper(void);

		// BApplication Hooks					
		virtual void		MessageReceived(BMessage *msg);
		virtual void		ReadyToRun(void);
	
		// Feed Handler Hooks
		virtual void		FeedSubscribed(Feed *feed);
		virtual void		ChannelUpdated(Feed *feed, Channel *channel);
		
	private:
		void				BroadcastChannelContents(Channel *channel);
	
		FeedListener		*fFeedListener;
		BMessenger			*fInfoPopper;

		entry_ref			fServerRef;
};

#endif
