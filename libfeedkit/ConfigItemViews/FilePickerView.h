#ifndef LIBFEEDKIT_FILEPICKERVIEW_H
#define LIBFEEDKIT_FILEPICKERVIEW_H

#include "ConfigItemView.h"

#include <vector>

class BButton;
class BRefFilter;
class BScrollView;
class DragListView;

namespace FeedKit {
	namespace Settings {
		
		class FilePickerView : public ConfigItemView {
			public:
									FilePickerView(const char *name, const char *label,
										display_type displayType, int32 dataType, float width,
										float divider, BMessage configItem, BMessage settings);
									~FilePickerView(void);
			
				// ConfigItemView hooks
				void				Revert(void);
				void				Defaults(void);
				void				Save(BMessage &settings);
				
				// BView Hooks
				void				AttachedToWindow(void);
				void				GetPreferredSize(float *width, float *height);
				void				MessageReceived(BMessage *msg);
				
			private:
				void				BuildContents(bool useDefault);
			
				BMessage			fSettings;
				BButton				*fButtonPick;
				BButton				*fButtonDelete;
				BRefFilter			*fRefFilter;
				DragListView		*fListView;
				BScrollView			*fScrollView;
		};
	};
};

#endif
