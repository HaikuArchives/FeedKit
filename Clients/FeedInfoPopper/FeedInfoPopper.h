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
		virtual void		ReadyToRun(void);
	
		// Feed Handler Hooks
		virtual void		FeedRegistered(Feed *feed);
		virtual void		ChannelUpdated(Feed *feed, Channel *channel);

		virtual void		EnclosureDownloadStarted(Feed *feed, Item *item, Enclosure *enclosure, entry_ref ref);
		virtual void		EnclosureDownloadProgress(Feed *feed, Item *item, Enclosure *enclosure, DownloadProgress *progress, entry_ref ref);
//		virtual void		EnclosureDownloadFinished(Feed *feed, Item *item, Enclosure *enclosure, entry_ref ref);
//		virtual void		EnclosureDownloadError(Feed *feed, Item *item, Enclosure *enclosure, Error *error);
		
	private:
		void				BroadcastChannelContents(Channel *channel);
		void				BroadcastDownloadProgress(Feed *feed, Item *item, Enclosure *enclosure, DownloadProgress *progress);
	
		FeedListener		*fFeedListener;
		BMessenger			*fInfoPopper;

		entry_ref			fServerRef;
};

#endif
