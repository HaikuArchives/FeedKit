#ifndef ENCLOSUREREQUESTHANDLER_H
#define ENCLOSUREREQUESTHANDLER_H

#include "FileRequestHandler.h"

class FeedServer;

class EnclosureRequestHandler : public FileRequestHandler {
	public:
							EnclosureRequestHandler(FeedServer *server);
	
		// FileRequestHandler Hooks
		virtual void		DownloadProgress(FileRequest *request, void *data,
								double downTotal, double downCurrent);

	private:
		FeedServer			*fServer;
};

#endif
