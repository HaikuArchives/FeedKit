#ifndef EMPTYLISTENER_H
#define EMPTYLISTENER_H

#include <Application.h>

namespace FeedKit {
	class FeedListener;
};

class EmptyListener : public BApplication {
	public:
						EmptyListener(void);
						~EmptyListener(void);

		// BApplication Hooks					
		virtual void	MessageReceived(BMessage *msg);
		virtual void	ReadyToRun(void);
	
	private:
		FeedKit::FeedListener		*fRSS;
};

#endif
