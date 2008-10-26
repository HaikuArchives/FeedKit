#include <libfeedkit/FeedListener.h>
#include <libfeedkit/TraceFeedHandler.h>

using namespace FeedKit;

int main(void) {	
	FeedListener *listener = new FeedListener();
	listener->StartListening();
	listener->AddHandler(new TraceFeedHandler());

	while (true) {
		snooze(10000);
	};

	return 0;
};
