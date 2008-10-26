#ifndef FEEDREGISTRATIONEWINDOW_H
#define FEEDREGISTRATIONWINDOW_H

#include <Window.h>

class BBox;
class BButton;
class BCheckBox;
class BMessenger;
class BMessage;
class BTextControl;
class BView;

class FeedRegistrationWindow : public BWindow {
	public:
								FeedRegistrationWindow(BMessenger *target, BMessage *cancel,
									BMessage *subscribe);
								~FeedRegistrationWindow(void);

		// BWindow Hooks
		bool					QuitRequested(void);
		void			 		MessageReceived(BMessage *msg);
		
	private:
		BMessenger				*fTarget;
		BMessage				*fMsgCancel;
		BMessage				*fMsgSubscribe;
		
		BView					*fView;
		BBox					*fBox;
		BTextControl			*fName;
		BTextControl			*fURL;
		BCheckBox				*fAutoUpdate;
		
		BButton					*fSubscribe;
		BButton					*fCancel;
};

#endif
