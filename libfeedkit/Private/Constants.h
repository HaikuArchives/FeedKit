#ifndef LIBFEEDKIT_PRIVATE_CONSTANTS_H
#define LIBFEEDKIT_PRIVATE_CONSTANTS_H

#include <String.h>

#include <vector>
#include <map>

namespace FeedKit {
	namespace Private {
		enum msg_what {
			Success = 'fp00',					// Used internally to indicate success
			AddListener = 'fp01',				// ... to add a listener
			RemoveListener = 'fp02',			// ... to remove a listener
			SettingsTemplateUpdated = 'fp03',	// ... apps settings template has been updated
		};

		namespace FromServer {
			enum msg_what {
				RegisterFeedError = 'rfs2',			// Indicates an error subscribing to a feed
				RegisterFeedComplete = 'rfs1',		// Indicates a feed has been subscribed to
				ChannelUpdated = 'fkcu',			// Channel updated
				ChannelIconUpdated = 'fkiu',		// Channel icon updated
				ItemRead = 'rfir',					// Item have been read
				
				DownloadEnclosureStarted = 'rfe1',	// An enclosure has started downloading
				DownloadEnclosureProgress = 'rfe2',	// ... progress for enclosure
				DownloadEnclosureFinished = 'rfe3', // Enclosure downloaded
				DownloadEnclosureError = 'rfe4',	// Error downloading enclosure

				ChannelIconPath = 'fkci',			// The channel icon is attached

				DownloadFeedStarted = 'Fdfs',		// Generic (feed, img, etc) file has started d/l
				DownloadFeedProgress = 'Fdfp',		// ... progress
				DownloadFeedFinished = 'Fdff',		// Generic file downloaded
				DownloadFeedError = 'Fdfe',			// Error downloading file
				
				FeedSubscriptionChanged = 'Ffsc',	// Feed has been [un]subscribed to
				
				CancelEnclosureDownloadError = 'cede',
													// Error cancelling download
				EnclosureDownloadStatusChanged = 'edsc',
													// The download status of an
													//   Enclosure has changed
			};
		};

		// The ToServer namespace details messages you may send to the Feed Server
		namespace ToServer {
			enum msg_what {
				ForceRefresh = 'ft01',				// Send this to force the refreshing of feeds
				RegisterFeed = 'ft02',				// Let the Feed Server know about a new feed
				DownloadEnclosure = 'ft03',			// Download the enclosure for the item
				CancelEnclosureDownload = 'ft04',	// Cancel the download of an Enclosure
				GetFeedList = 'ft05',				// Returns the list of Feeds subscribed to
				MarkRead = 'ft06',					// Mark the attached Items as read
				GetChannelIconPath = 'ft07',		// Return the file path for a local
													//	copy of the channel's icon
				ChangeFeedSubscription = 'ft08',	// Sets if we should keep updating this Feed
			};
		};
		

		// The ServerReply namespace details messages you'll get in response to ToServer messages
		namespace ServerReply {
			enum msg_what {
				Error = 'ffre',						// Error!
				FeedList = 'ffrl',					// A list of feeds interested in
			};
		};
	};
};

#endif
