#ifndef PAPPLICATION_H
#define PAPPLICATION_H

#include "main.h"

#include <Application.h>
#include <Window.h>

class PWindow;
class BApplication;

class PApplication : public BApplication {
	public:
	
							PApplication(void);
							~PApplication(void);
		
		// BApplication Hooks				
		bool				QuitRequested(void);
		void				MessageReceived(BMessage *msg);
		void				ReadyToRun(void);
				
	private:
		PWindow				*fWindow;
};

#endif

