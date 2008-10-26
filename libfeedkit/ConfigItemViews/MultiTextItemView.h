#ifndef LIBFEEDKIT_MULTITEXTITEMVIEW_H
#define LIBFEEDKIT_MULTITEXTITEMVIEW_H

#include "ConfigItemView.h"

class BTextView;
class BScrollView;

namespace FeedKit {
	namespace Settings {
		class MultiTextItemView : public ConfigItemView {
			public:
									MultiTextItemView(const char *name, const char *label,
										display_type displayType, int32 dataType, float width,
										float divider, BMessage configItem, BMessage settings);
									~MultiTextItemView(void);
			
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
				BTextView			*fTextView;
				BScrollView			*fScrollView;
		};
	};
};

#endif
