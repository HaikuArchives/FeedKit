#include <Application.h>
#include <libfeedkit/FeedKitConstants.h>
#include <libfeedkit/FeedListener.h>

namespace FeedKit {
	class FeedListener;
};

using namespace FeedKit;

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Usage is\n%s {URL} [{Friendly Name}]\n", argv[0]);
		return B_ERROR;
	};
	BApplication app("application/x-vnd.beclan.feedkit.registerfeed");
		
	const char *url = argv[1];
	const char *name = NULL;
		
	if (argc > 2) name = argv[2];
				
	FeedListener *server = new FeedListener();
	server->RegisterFeed(url, name);

	return 0;
};
