#ifndef LIBFEEDKIT_FEEDLISTENER_H
#define LIBFEEDKIT_FEEDLISTENER_H

#include <Looper.h>
#include <Messenger.h>

#include <libfeedkit/FeedKitConstants.h>

#include <vector>

namespace FeedKit {
	class FeedHandler;
};

typedef vector<FeedKit::FeedHandler *> handler_t;

namespace FeedKit {
	class Channel;
	class Feed;
	class Item;

	class FeedListener : public BLooper {
		public:
								FeedListener(void);
								~FeedListener(void);
								
			// BLooper Hooks
			void				MessageReceived(BMessage *msg);
			status_t			InitCheck(void);
			
			void				StartListening(void);
			void				StopListening(void);
			status_t			SendMessage(BMessage *msg, BMessage *reply = NULL);
			bool				ServerIsRunning(void);

			// Server Interaction Methods
			status_t			RefreshAllFeeds(void);
			status_t			RefreshFeed(Feed *feed);
			status_t			MarkItemRead(Item *item);
			status_t			RegisterFeed(const char *url, const char *name = NULL);
			status_t			ChangeFeedSubscription(Feed *feed, bool subscribed);
			const char			*ChannelIconPath(Channel *channel);
			feed_list_t			Feeds(void);
			status_t			DownloadEnclosure(Item *item, Enclosure *enclosure, const char *path);
			status_t			CancelEnclosureDownload(Item *item, Enclosure *enclosure);
	
			// Handler Subscription
			void				AddHandler(FeedHandler *handler);
			void				RemoveHandler(FeedHandler *handler);
	
		private:
			void				AddListener(BMessenger target);
			void				RemoveListener(BMessenger target);
			
			bool				fListening;
			bool				fRegistered;
			
			BMessenger			fMsgr;
			BMessenger			fTarget;
			
			handler_t			fHandler;
	};
};

#endif
