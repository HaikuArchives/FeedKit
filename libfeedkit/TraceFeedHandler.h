#ifndef LIBFEEDKIT_TRACEFEEDHANDLER_H
#define LIBFEEDKIT_TRACEFEEDHANDLER_H

#include <libfeedkit/FeedHandler.h>

#include <stdio.h>

namespace FeedKit {
	class Channel;
	class DownloadProgress;
	class Enclosure;
	class ErrorDetails;
	class Feed;
	class Item;

	class TraceFeedHandler : public FeedHandler {
		public:
								TraceFeedHandler(FILE *output = stderr);
			virtual				~TraceFeedHandler(void);
		
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
		
			FILE				*fOutput;
	};
};

#endif
