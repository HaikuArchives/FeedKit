#ifndef ITEMREADER_H
#define ITEMREADER_H

#include <Application.h>

namespace FeedKit {
	class FeedListener;
};

using namespace FeedKit;

class ItemReader : public BApplication {
	public:
									ItemReader(void);
									~ItemReader(void);

		// BApplication Hooks					
		virtual void				ReadyToRun(void);
		virtual void				RefsReceived(BMessage *msg);
	
	private:
		void						LaunchItem(entry_ref ref);
		void						MarkAsRead(entry_ref ref);
		
		FeedListener				*fFeed;
};

#endif
