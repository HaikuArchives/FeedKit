#ifndef LIBFEEDKIT_CONSTANTS_H
#define LIBFEEDKIT_CONSTANTS_H

#include <String.h>

#include <vector>

namespace FeedKit {
	class Channel;
	class Content;
	class Enclosure;
	class Feed;
	class Item;

	extern const char *ServerSignature;
	
	typedef std::vector<BString> uuid_list_t;

	typedef std::vector<Content *> content_list_t;
	typedef std::vector<Channel *> channel_list_t;
	typedef std::vector<Enclosure *> enclosure_list_t;
	typedef std::vector<Feed *> feed_list_t;
	typedef std::vector<Item *> item_list_t;

	namespace FromServer {
		enum msg_what {
			SettingsUpdated = 'rfsu',			// Settings have been updated
		};
	};

	namespace ErrorCode {
		enum code {
			UnableToParseFeed = 'ecpf',			// Unable to parse the Feed
			InvalidItem = 'ecii',				// Invalid Item
			InvalidEnclosure = 'ecie',			// Invalid Enclosure
			InvalidEnclosurePath = 'ecip',		// Invalid Enclosure path
			UnableToStartDownload = 'ecsd',		// Unable to start downloading the Enclosure
		};
	};

	// The Settings namespace details things relating to settings and settings templates
	namespace Settings {
		enum display_type {
			Unset = '    ',						// Unset - don't use, used internally
			Hidden = 'hide',					// A setting not controlled by the user
			RadioButton = 'rabu',				// Radio buttons
			CheckBox = 'chkb',					// Check boxes
			MenuSingle = 'mens',				// Single-select menu
			MenuMulti = 'menm',					// Multiple-select menu
			TextSingle = 'txts',				// Single line text control
			TextMulti = 'txtm',					// Multi line text control
			TextPassword = 'txtp',				// Single line, hidden input, text control
			FilePickerSingle = 'fpks',			// File Picker - single file
			FilePickerMulti = 'fpkm',			// File Picker - multiple file
			DirectoryPickerSingle = 'dpks',		// Directory Picker - single
			DirectoryPickerMultiple = 'dpkm',	// Directory picker - multiple
		};

		// These are some common types of applications. The Type param of FeedListener is free
		// form but you should use one of these constants, if possible, to ensure consistency
		namespace AppTypes {
			extern const char *SettingClient;		// Something which allows user interaction
			extern const char *SettingServer;		// The server
			extern const char *SettingUtil;			// Interacts with the FeedKit but not the
													// 	user
			extern const char *SettingParser;		// An addon for parsing data into FeedKit
													//	objects
			extern const char *SettingFeed;			// Settings specific to a feed
		};
		
		namespace Icon {
			enum Location {
				Contents = 'cnts',					// Icon comes from the contents of the file
				TrackerIcon = 'trki',				// Use the Tracker icon
			};
		};
	};
};

#endif
