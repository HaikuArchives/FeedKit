#ifndef LIBFEEDKIT_MENUITEMVIEW_H
#define LIBFEEDKIT_MENUITEMVIEW_H

#include "ConfigItemView.h"

class BMenu;
class BMenuField;

namespace FeedKit {
	namespace Settings {
		class MenuItemView : public ConfigItemView {
			public:
									MenuItemView(const char *name, const char *label,
										display_type displayType, int32 dataType, float width,
										float divider, BMessage configItem, BMessage settings);
									~MenuItemView(void);
			
				// ConfigItemView hooks
				void				Revert(void);
				void				Defaults(void);
				void				Save(BMessage &settings);
				
				// BView Hooks
				void				AttachedToWindow(void);
				void				GetPreferredSize(float *width, float *height);
				void				MessageReceived(BMessage *msg);
		
			private:
				void				BuildMenu(bool defaultOnly);
			
				BMessage			fSettings;
				BMenuField			*fMenuField;
				BMenu				*fMenu;
		};
	};
};

#endif
