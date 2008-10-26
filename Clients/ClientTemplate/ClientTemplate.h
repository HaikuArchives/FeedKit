#ifndef CLIENTTMEPLATE_H
#define CLIENTTEMPLATE_H

#include <Application.h>

#include <libfeedkit/FeedHandler.h>

namespace FeedKit {
	class FeedListener;
};

using namespace FeedKit;

class ClientTemplate : public BApplication, public FeedHandler {
	public:
							ClientTemplate(void);
							~ClientTemplate(void);

		// BApplication Hooks					
		void				MessageReceived(BMessage *msg);
		void				ReadyToRun(void);
		bool				QuitRequested(void);
	
	private:
		FeedListener		*fFeed;
};

#endif
