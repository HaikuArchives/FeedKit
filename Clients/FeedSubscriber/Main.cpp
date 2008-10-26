#include <Application.h>
#include <libfeedkit/FeedKitConstants.h>
#include <libfeedkit/FeedListener.h>
#include <libfeedkit/Feed.h>

using namespace FeedKit;

int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("Usage is\n%s {URL} {subscribe | unsubscribe}\n", argv[0]);
		return B_ERROR;
	};
	BApplication app("application/x-vnd.beclan.feedkit.feedsubscriber");
		
	BString url = argv[1];
	bool subscribe = (strcmp(argv[2], "subscribe") == 0);
					
	FeedListener *server = new FeedListener();
	feed_list_t feeds = server->Feeds();
	bool found = false;
	
	for (feed_list_t::iterator it = feeds.begin(); it != feeds.end(); it++) {
		Feed *feed = (*it);
		
		if (url.ICompare(feed->URL()) == 0) {
			found = true;
			server->ChangeFeedSubscription(feed, subscribe);
			
			printf("Feed subscription changed\n");
			
			break;
		};
	};
	
	if (found == false) {
		printf("Error - unable to find feed\n");
	};

	return (found == true ? B_OK : B_ERROR);
};
