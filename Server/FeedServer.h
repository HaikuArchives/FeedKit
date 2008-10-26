#ifndef FEEDSERVER_H
#define FEEDSERVER_H

#include <Application.h>
#include <Messenger.h>
#include <Path.h>

#include <map>
#include <vector>

#include <libfeedkit/FeedKitConstants.h>

#include "ObjectSource.h"

class FeedServer;
class BMessageRunner;
class BVolumeRoster;
class SettingsFile;
class QueryLooper;
class ParserManager;
class FileDaemon;
class FileRequest;

namespace FeedKit {
	class Feed;
	class Channel;
	class Item;
	class DownloadProgress;
};

typedef map<BString, entry_ref> ref_uuid_t;
typedef map<entry_ref, FeedKit::Feed *> reffeed_t;
typedef map<BString, FeedKit::Feed *> urlfeed_t;
typedef map<BString, SettingsFile *> feedsetting_t;
typedef vector<BMessenger> listener_t;
typedef map<BString, BMessageRunner *> refresh_t;
typedef map<BString, FeedKit::Feed *> feed_uuid_t;
typedef map<BString, FeedKit::Channel *> channel_uuid_t;
typedef map<BString, FeedKit::Item *> item_uuid_t;
typedef map<BString, FeedKit::Enclosure *> enclosure_uuid_t;
typedef map<BString, BString> channel_icon_t;
typedef map<BString, entry_ref> existing_enclosure_t;
typedef status_t (FeedServer::*EventHandler)(BMessage *);
typedef map<int32, EventHandler> eventhandler_t;

class FeedServer : public BApplication, public ObjectSource {


	public:
									FeedServer(void);
									~FeedServer(void);

		// BApplication Hooks					
		void						MessageReceived(BMessage *msg);
		void						ReadyToRun(void);

		// BLooper Hooks
		bool						QuitRequested(void);
	
		// ObjectSource Hooks
		FeedKit::Feed				*GetFeed(const char *uuid);
		FeedKit::Channel			*GetChannel(const char *uuid);
		FeedKit::Item				*GetItem(const char *uuid);
		FeedKit::Enclosure			*GetEnclosure(const char *uuid);
		
		// Public
		void						UpdateEnclosureProgress(FileRequest *request,
										FeedKit::Enclosure *enc, double downloaded);
	private:
		void						LaunchFeed(entry_ref ref);
		status_t					SyndicateURL(const char *url, const char *name = "");
		FeedKit::DownloadProgress	*ProgressFromRequest(FileRequest *request);
		void						MarkItems(FeedKit::Feed *feed, bool isCurrent, bool isNew);
		void						StartAutoStartClients(void);
		void						StopAutoStartClients(void);
	
		void						Broadcast(BMessage *msg);
		void						AddListener(BMessenger msgr);
		void						RemoveListener(BMessenger msgr);
		
		void						UpdateReferences(FeedKit::Feed *feed);
		
		FeedKit::Feed 				*MergeFeed(FeedKit::Feed *oldFeed, FeedKit::Feed *newFeed);
		FeedKit::Channel 			*MergeChannel(FeedKit::Feed *feed, FeedKit::Channel *oldChannel,
										FeedKit::Channel *newChannel);
		void						UpdateFeedUUIDs(FeedKit::Feed *feed);

		void						FindExistingEnclosureDownloads(FeedKit::Feed *feed);
		status_t					DownloadEnclosure(FeedKit::Feed *feed,
										FeedKit::Channel *channel, FeedKit::Item *item,
										FeedKit::Enclosure *enclosure, entry_ref ref,
										int32 offset = 0);

		void						CheckIndexes(void);
		void						SaveFeeds(void);

		// Event Handlers
		status_t					HandleCheckDownloads(BMessage *msg);
		status_t					HandleChannelQuery(BMessage *msg);
		status_t					HandleEnclosureQuery(BMessage *msg);
		status_t					HandleFeedDownloadStart(BMessage *msg);
		status_t					HandleFeedDownloadError(BMessage *msg);
		status_t					HandleFeedDownloadFinish(BMessage *msg);
		status_t					HandleForceRefresh(BMessage *msg);
		status_t					HandleMarkRead(BMessage *msg);
		status_t					HandleDownloadEnclosure(BMessage *msg);
		status_t					HandleCancelEnclosureDownload(BMessage *msg);
		status_t					HandleEnclosureDownloadStart(BMessage *msg);
		status_t					HandleEnclosureDownloadFinish(BMessage *msg);
		status_t					HandleEnclosureDownloadError(BMessage *msg);
		status_t					HandleEnclosureDownloadCancelled(BMessage *msg);
		status_t					HandleRegisterFeed(BMessage *msg);
		status_t					HandleGetFeedList(BMessage *msg);
		status_t					HandleGetChannelIconPath(BMessage *msg);
		status_t					HandleAddListener(BMessage *msg);
		status_t					HandleRemoveListener(BMessage *msg);
		status_t					HandleSettingsTemplateUpdated(BMessage *msg);
		status_t					HandleNodeMonitor(BMessage *msg);
		status_t					HandleChannelIconDownloadStart(BMessage *msg);
		status_t					HandleChannelIconDownloadFinish(BMessage *msg);
		status_t					HandleChannelIconDownloadError(BMessage *msg);
		status_t					HandleChangeFeedSubscription(BMessage *msg);
		status_t					HandleFlattenFeeds(BMessage *msg);

		QueryLooper					*fFeedQuery;
		QueryLooper					*fEnclosureQuery;

		ParserManager				*fParserManager;
	
		FileDaemon					*fFeedDaemon;
		reffeed_t					fRefFeeds;
		ref_uuid_t					fRefFeedUUID;
		urlfeed_t					fURLFeeds;
		
		// UUID Lookups
		feed_uuid_t					fFeedUUID;
		channel_uuid_t				fChannelUUID;
		item_uuid_t					fItemUUID;
		enclosure_uuid_t			fEnclosureUUID;
			
		channel_icon_t				fChannelIcon;
			
		SettingsFile				*fSettings;
		feedsetting_t				*fFeeds;
		listener_t					fListeners;
		
		int32						fRefresh;
		BMessageRunner				*fDownloadStatusRunner;
		refresh_t					fRefreshRunners;
		BMessageRunner				*fFlattenFeedRunner;
		
		BPath						fSettingsPath;
		BVolumeRoster				*fVolRoster;
		
		existing_enclosure_t		fDownloadedEnclosure;
		
		eventhandler_t				fHandler;
};

#endif
