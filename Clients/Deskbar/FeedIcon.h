#ifndef FEEDDESKBAR_H
#define FEEDDESKBAR_H

#include <Message.h>
#include <View.h>

#include <map>

#include <libfeedkit/FeedHandler.h>
#include <libfeedkit/ItemSpecification.h>

class BBitmap;
class BMenu;
class BPopUpMenu;

namespace FeedKit {
	class FeedListener;
	class Channel;
	
	namespace Settings {
		class SettingsManager;
	};
};

using namespace FeedKit;
using namespace FeedKit::Settings;

class FeedIcon : public BView, public FeedHandler {
	public:
							FeedIcon(void);
							FeedIcon(BMessage *archive);
							~FeedIcon(void);

		// BArchivable Hooks
		status_t			Archive(BMessage *archive, bool deep = true) const;
		static BArchivable	*Instantiate(BMessage *archive);
		
		// BView hooks
		void				Draw(BRect rect);
		void				MouseDown(BPoint point);
		void				AttachedToWindow(void);
		void				DetachedFromWindow(void);
		
		void				MessageReceived(BMessage *msg);
		
		// Feed Handler Hooks
		void				ServerStarted(void);
		void				ServerShutdown(void);
	
		void				FeedRegistered(Feed *feed);
		void				FeedSubscriptionChanged(Feed *feed);
		void				ChannelUpdated(Feed *feed, Channel *channel);
		void				ItemRead(Feed *feed, Channel *channel, Item *item);
	
	private:
		void				Init(void);
		BBitmap				*ReadNodeIcon(BString &path, int32 size, bool followLink = true);
		BMenu				*FeedMenu(void);
		BMenu				*ChannelMenuItems(Channel *channel);
		void				Rebuild(void);
		bool				HasUnreadItems(void);
		void				HandleSettings(BMessage *settings);
			
		FeedListener		*fFeed;
		SettingsManager		*fSettingsMan;
		BMessage			fSettings;
		BBitmap				*fOfflineIcon;
		BBitmap				*fNormalIcon;
		BBitmap				*fNewItemsIcon;
		BBitmap				*fIcon;
		BPopUpMenu			*fMenu;
		BMenu				*fFeedMenu;
		bool				fDirtyMenu;
		ItemSpecification	*fMenuSpecification;
};

#endif
