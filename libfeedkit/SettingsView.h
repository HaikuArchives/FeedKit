#ifndef LIBFEEDKIT_SETTINGSVIEW_H
#define LIBFEEDKIT_SETTINGSVIEW_H

#include <libfeedkit/FeedKitConstants.h>

#include <Message.h>
#include <View.h>

#include <vector>

class BCheckBox;
class BubbleHelper;

namespace FeedKit {
	namespace Settings {

		class ConfigItemView;
	
		typedef std::vector<ConfigItemView *> setting_t;
		
		class SettingsView : public BView {
			public:
									SettingsView(BRect frame, const char *appType, const char *name,
										uint32 resizing, uint32 flags, BMessage tmplate,
										BMessage settings, BubbleHelper *helper = NULL);
									~SettingsView(void);
									
				// BView Hooks
				void				AttachedToWindow(void);
				void				MessageReceived(BMessage *msg);
				void				GetPreferredSize(float *width, float *height);
		
				// Public
				void				Save(BMessage &settings);
				void				Revert(void);
				void				Default(void);
				
			private:
				void				BuildGUI(BMessage tmplate, BMessage settings);
				
				BCheckBox			*fAutoStart;
				BString				fAppType;
				
				BMessage			fTemplateMsg;
				BMessage			fSettingMsg;
				
				setting_t			fSettings;
				BubbleHelper		*fHelper;
		};
	};
};

#endif
