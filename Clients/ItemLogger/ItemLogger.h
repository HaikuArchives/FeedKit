#ifndef ITEMLOGGER_H
#define ITEMLOGGER_H

#include <Application.h>

class QueryLooper;

namespace FeedKit {
	class Channel;
	class Item;
	class FeedListener;

	namespace Settings {
		class SettingsManager;
	};
};

using namespace FeedKit;
using namespace FeedKit::Settings;

class ItemLogger : public BApplication {
	public:
						ItemLogger(void);
						~ItemLogger(void);

		// BApplication Hooks					
		virtual void	MessageReceived(BMessage *msg);
		virtual void	ReadyToRun(void);
	
	private:
		void			Flatten(const char *feedurl, Channel *channel);
		status_t		SaveItem(const char *path, Channel *channel, Item *item);
		BString			MakeURLSafe(const char *url);
		
	
		FeedListener	*fFeed;
		QueryLooper		*fQuery;
		SettingsManager	*fSettings;
};

#endif
