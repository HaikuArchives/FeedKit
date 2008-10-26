#ifndef LIBFEEDKIT_FEEDHANDLER_H
#define LIBFEEDKIT_FEEDHANDLER_H

#include <Entry.h>

namespace FeedKit {
	class Channel;
	class DownloadProgress;
	class Enclosure;
	class ErrorDetails;
	class Feed;
	class Item;

	class FeedHandler {
		public:
								FeedHandler(void);
			virtual				~FeedHandler(void);
		
			// Overridable Hooks
			virtual void		ServerStarted(void);
			virtual void		ServerShutdown(void);
			
			virtual void		FeedRegistered(Feed *feed);
			virtual void		FeedRegisteredError(Feed *feed, ErrorDetails *error);
			virtual void		FeedSubscriptionChanged(Feed *feed);

			virtual void		ChannelUpdated(Feed *feed, Channel *channel);
			virtual void		ChannelIconUpdated(Feed *feed, Channel *channel, entry_ref ref);

			virtual void		ItemRead(Feed *feed, Channel *channel, Item *item);

			virtual void		EnclosureDownloadStarted(Feed *feed, Item *item, Enclosure *enclosure);
			virtual void		EnclosureDownloadProgress(Feed *feed, Item *item, Enclosure *enclosure);
			virtual void		EnclosureDownloadFinished(Feed *feed, Item *item, Enclosure *enclosure);
			virtual void		EnclosureDownloadError(Feed *feed, Item *item, Enclosure *enclosure, ErrorDetails *error);
			virtual void		EnclosureDownloadStatusChanged(Feed *feed, Item *item, Enclosure *enclosure);

			virtual void		FeedDownloadStarted(Feed *feed, DownloadProgress *progress);
			virtual void		FeedDownloadProgress(Feed *feed, DownloadProgress *progress);
			virtual void		FeedDownloadFinished(Feed *feed, DownloadProgress *progress);
			virtual void		FeedDownloadError(Feed *feed, DownloadProgress *progress);

		private:
	};
};

#endif
