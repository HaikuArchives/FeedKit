#ifndef LIBFEEDKIT_SETTINGSMANAGER_H
#define LIBFEEDKIT_SETTINGSMANAGER_H

#include <Entry.h>
#include <Invoker.h>
#include <NodeMonitor.h>
#include <Path.h>
#include <String.h>

#include <libfeedkit/FeedKitConstants.h>

class BBitmap;

namespace FeedKit {
	namespace Settings {

		class SettingsManager : public BInvoker {
			public:
									SettingsManager(const char *type, const char *app);
									~SettingsManager(void);
									
				const char			*Type(void);
				const char			*App(void);
				
				status_t			DisplayName(const char *name);
				const char			*DisplayName(void);
				
				BMessage			Settings(void);
				status_t			Settings(BMessage *settings);

				BMessage			Template(void);
				status_t			Template(BMessage *tmplate, const char *appsig = NULL);
				
				status_t			WatchSettings(uint32 flags = B_WATCH_STAT | B_WATCH_ATTR);
									
				BBitmap				*Icon(int32 size);
				status_t			Icon(entry_ref source, Icon::Location location);
			
			private:
				BString				fType;
				BString				fApp;
				BString				fDisplayName;
				
				BPath				fSettingsPath;
				entry_ref			fSettings;
				entry_ref			fTemplate;
		};
	};
};

#endif
