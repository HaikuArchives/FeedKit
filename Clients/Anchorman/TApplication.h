#ifndef TAPPLICATION_H
#define TAPPLICATION_H

#include <Application.h>

class TWindow;

class TApplication : public BApplication {
	public:
	
							TApplication(bool unarchive = true);
							~TApplication(void);
		
		// BApplication Hooks				
		bool				QuitRequested(void);
		void				MessageReceived(BMessage *msg);
		void				ReadyToRun(void);
				
	private:
		bool				fUnarchive;
		TWindow				*fWindow;
};

#endif

