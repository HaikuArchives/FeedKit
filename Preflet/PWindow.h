#ifndef PWINDOW_H
#define PWINDOW_H

#include "main.h"

#include <Path.h>
#include <String.h>
#include <Window.h>

#include <map>

class BBox;
class BButton;
class BDirectory;
class BOutlineListView;
class BView;
class IconTextItem;
class BubbleHelper;

namespace FeedKit {
	namespace Settings {
		class SettingsView;
	};
};

using namespace FeedKit;
using namespace FeedKit::Settings;

class ViewInfo;

typedef map<BString, ViewInfo *> pref_t;

class PWindow : public BWindow {
	public:
		
								PWindow(void);
								~PWindow(void);

		// BWindow Hooks
		bool					QuitRequested(void);
		void					MessageReceived(BMessage *msg);

				
	private:
		float					ExtractSettings(IconTextItem *parent, BRect frame,
									BDirectory *templateDir, const char *apptype);
	
		BView					*fView;
		BButton					*fSave;
		BButton					*fRevert;
		BButton					*fDefault;
		BOutlineListView		*fListView;
		BBox					*fBox;

		pref_t					fPrefView;
		SettingsView 			*fCurrentView;

		float					fFontHeight;
		BubbleHelper			*fHelper;
		BPath					fSettingPath;
};

#endif
