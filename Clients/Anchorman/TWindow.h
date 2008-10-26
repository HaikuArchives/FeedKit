#ifndef TWINDOW_H
#define TWINDOW_H

#include <String.h>
#include <Window.h>

#include <libfeedkit/FeedHandler.h>

#include "RowTag.h"

#include <map>

class BArchivable;
class BBox;
class BButton;
class BColumnListView;
class BDragger;
class BMenu;
class BMessageRunner;
class BShelf;
class BRow;
class BView;
class ResizeView;
class FeedRegistrationWindow;

namespace FeedKit {
	class Channel;
	class Feed;
	class FeedListener;
	class ObjectCache;
	
	namespace Settings {
		class SettingsManager;
	};
};

typedef RowTag<BString> row_t;
typedef map<BString, row_t *> feed_row_t;

class TWindow : public BWindow, FeedKit::FeedHandler {
	public:	
								TWindow(void);
								TWindow(BMessage *archive);
								~TWindow(void);

		// BWindow Hooks
		bool					QuitRequested(void);
		void					MessageReceived(BMessage *msg);
		
		// BArchivable Hooks
		status_t				Archive(BMessage *archive, bool deep = true) const;
		static BArchivable		*Instantiate(BMessage *archive);

		// FeedHandler Hooks
		virtual void			ServerStarted(void);
		virtual void			ServerShutdown(void);

		virtual void			FeedRegistered(FeedKit::Feed *feed);
		virtual void			FeedSubscriptionChanged(FeedKit::Feed *feed);
		virtual void			ChannelUpdated(FeedKit::Feed *feed, FeedKit::Channel *channel);

		virtual void			ItemRead(FeedKit::Feed *feed, FeedKit::Channel *channel, FeedKit::Item *item);

		virtual void			EnclosureDownloadStarted(FeedKit::Feed *feed, FeedKit::Item *item, FeedKit::Enclosure *enclosure);
		virtual void			EnclosureDownloadProgress(FeedKit::Feed *feed, FeedKit::Item *item, FeedKit::Enclosure *enclosure);
		virtual void			EnclosureDownloadFinished(FeedKit::Feed *feed, FeedKit::Item *item, FeedKit::Enclosure *enclosure);
		virtual void			EnclosureDownloadError(FeedKit::Feed *feed, FeedKit::Item *item, FeedKit::Enclosure *enclosure, FeedKit::ErrorDetails *error);
		virtual void			EnclosureDownloadStatusChanged(FeedKit::Feed *feed, FeedKit::Item *item, FeedKit::Enclosure *enclosure);

	private:
		void					Init(void);
		void					RebuildFeedList(void);
	
		void					ClearFeedList(void);
		void					ClearItemList(void);
		void					ClearContentView(void);
		void					ClearEnclosureList(void);
		void					ClearItemRunner(void);
	
		void					UpdateEnclosureStatus(FeedKit::Item *item, FeedKit::Enclosure *enclosure, int32 percentage, entry_ref ref);
		
		BBitmap					*ReadNodeIcon(BString &path, int32 size, bool followLink = true);
	
		void					VisitURL(const char *url);
	
		FeedKit::FeedListener	*fFeedListener;
		FeedKit::Settings::SettingsManager
								*fSettingsManager;

		FeedKit::Channel		*fSelectedChannel;
		FeedKit::Item			*fSelectedItem;

		BMessageRunner			*fItemReadRunner;

		BBitmap					*fIconAttachment;
		BBitmap					*fIconNew;
		BBitmap					*fIconArchived;
		BBitmap					*fIconUnread;
		BBitmap					*fIconSubscribed;

		BColumnListView			*fChannelList;
		BBox					*fBox;
		BColumnListView			*fItemList;
		BBox					*fContentEnclosureBox;
		BMenu					*fContentMenu;
		BView					*fNetPositive;
		BColumnListView			*fEnclosureList;
		
		feed_row_t				fFeedRow;
		FeedKit::ObjectCache	*fCache;
		
		FeedRegistrationWindow	*fSubscribe;
};

#endif
