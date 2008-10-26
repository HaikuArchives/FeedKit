#ifndef LIBFEEDKIT_TEXTITEMVIEW_H
#define LIBFEEDKIT_TEXTITEMVIEW_H

#include "../ConfigItemView.h"

class BTextControl;

namespace FeedKit {
	namespace Settings {
		class TextItemView : public ConfigItemView {
			public:
									TextItemView(const char *name, const char *label,
										display_type displayType, int32 dataType, float width,
										float divider, BMessage configItem, BMessage settings);
									~TextItemView(void);
			
				// ConfigItemView hooks
				void				Revert(void);
				void				Defaults(void);
				void				Save(BMessage &settings);
				
				// BView Hooks
				void				AttachedToWindow(void);
				void				GetPreferredSize(float *width, float *height);
			private:
				BString				ExtractValue(bool useDefault);
			
				BMessage			fSettings;
				BTextControl		*fTextControl;
		};
	};
};
#endif
