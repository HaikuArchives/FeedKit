#include "EnclosureRequestHandler.h"
#include "FeedServer.h"

#include <libfeedkit/DownloadProgress.h>
#include <libfeedkit/Enclosure.h>

#include <stdio.h>
#include <stdlib.h>

//#pragma mark Namespace

using namespace FeedKit;

//#pragma mark Constructr

EnclosureRequestHandler::EnclosureRequestHandler(FeedServer *server)
	: fServer(server) {
};

//#pragma mark FileRequestHandler Hooks

void EnclosureRequestHandler::DownloadProgress(FileRequest *request, void *data, double downTotal, double downCurrent) {
	fprintf(stderr, "EnclosureRequestHandler::DownloadProgress([%p], [%p], %.2f, %.2f) called\n", request, data, downTotal, downCurrent);

	Enclosure *enclosure = reinterpret_cast<Enclosure *>(data);
	fServer->UpdateEnclosureProgress(request, enclosure, downCurrent);
};
