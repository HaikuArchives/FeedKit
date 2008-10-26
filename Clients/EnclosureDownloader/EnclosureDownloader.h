#ifndef ENCLOSUREDOWNLOADER_H
#define ENCLOSUREDOWNLOADER_H

#include <Application.h>

#include <libfeedkit/FeedHandler.h>

namespace FeedKit {
	class FeedListener;
	class Item;
	
	namespace Settings {
		class SettingsManager;
	};
};

using namespace FeedKit;
using namespace FeedKit::Settings;

class EnclosureDownloader : public BApplication, public FeedKit::FeedHandler {
	public:
							EnclosureDownloader(void);
							~EnclosureDownloader(void);

		// BApplication Hooks					
		virtual void		ReadyToRun(void);
		virtual void		MessageReceived(BMessage *msg);
	
		// FeedHandler Hooks
		virtual void		FeedRegistered(Feed *feed);
		void				ChannelUpdated(Feed *feed, Channel *channel);
		
	private:
		void				DownloadItemEnclosures(Item *item);
	
		FeedListener		*fFeed;
		SettingsManager		*fSettingsMan;
				
		entry_ref			fEnclosurePath;
};

#endif
