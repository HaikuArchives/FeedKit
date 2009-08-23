#ifndef LIBFEEDKIT_CONFIGITEMVIEW_H
#define LIBFEEDKIT_CONFIGITEMVIEW_H

#include <Message.h>
#include <String.h>
#include <View.h>

#include <libfeedkit/FeedKitConstants.h>

#include <vector>

namespace FeedKit {
	namespace Settings {
		typedef std::vector<BString> vec_str_t;
		typedef std::vector<int32> vec_int32_t;
		typedef std::vector<entry_ref> vec_ref_t;
		
		class ConfigItemView : public BView {
			public:
									ConfigItemView(const char *name, const char *label,
										display_type displayType, int32 dataType, float width,
										float divider, BMessage configItem, BMessage settings);
									~ConfigItemView(void);
				
				// BView hooks
				//	The ConfigItemView version of Draw draws the label in the correct location
				//		if you choose to do custom drawing, you should call ConfigItemView::Draw()
				//		to ensure the label is drawn correctly
				virtual void		Draw(BRect frame);
				//	You must implement GetPreferredSize() - the width should always return Width() -
				//		it'll generally be ignored, anyway. The height param should be filled in correctly
				virtual void		GetPreferredSize(float *width, float *height) = 0;
				
				// General accessor functions
				const char			*Name(void);
				const char			*Label(void);
				display_type		DisplayType(void);
				int32				DataType(void);
				float				Width(void);
				float				Divider(void);
				BMessage			*ConfigItem(void);
				const float			Padding(void) const;
				void				SetSettings(BMessage settings);
		
				// Various data extraction functions
				vec_str_t			SettingsString(void);
				vec_str_t			SettingsDefaultString(void);
				vec_int32_t			SettingsInt32(void);
				vec_int32_t			SettingsDefaultInt32(void);
				vec_ref_t			SettingsRef(void);
				vec_ref_t			SettingsDefaultRef(void);
		
				// These three hooks are used to save / restore state
				//	Revert - Reverts to the last saved copy of the settings
				//	Defaults - Reverts to the defaults specified in the configItem message
				//	Save - Writes the current settings into the supplied BMessage
				virtual void		Revert(void) = 0;
				virtual void		Defaults(void) = 0;
				virtual void		Save(BMessage &settings) = 0;
			
			private:
				BString				fName;
				BString				fLabel;
				display_type		fDisplayType;
				int32				fDataType;
				float				fWidth;
				float				fDivider;
				BMessage			fConfigItem;
				BMessage			fSettings;
				
				BPoint				fLabelLocation;
		};
	};
};

#endif
