#ifndef FILEDAEMON_H
#define FILEDAEMON_H

#include <Looper.h>
#include <OS.h>

#include <map>
#include <vector>

#include "FileRequestHandler.h"

class BMessageRunner;
class RequestInfo;
class FileRequest;

typedef std::vector<FileRequest *> request_t;
typedef std::vector<RequestInfo *> info_t;
typedef std::map<thread_id, RequestInfo *> thread_t;

class FileDaemon : public BLooper, public FileRequestHandler {
	public:
							FileDaemon(uint32 maxThreads);
							~FileDaemon(void);

		// BLooper Hooks
		void				MessageReceived(BMessage *msg);
		bool				QuitRequested(void);
	
		// FileRequestHandler Hooks
		virtual void		DownloadStarted(FileRequest *request, void *data);
	
		// Public 
		status_t			AddRequest(FileRequest *request, int32 priority = 0,
								BMessenger *target = NULL, BMessage *start = NULL,
								BMessage *finish = NULL, BMessage *error = NULL,
								BMessage *cancel = NULL);
		request_t			ActiveRequests(void);
		status_t			CancelDownload(const char *url);
		
	private:
		void				LaunchThread(void);
		static int32		WaitOnRequests(void *arg);
	
		uint32				fMaxThreads;
		BMessageRunner		*fThreadPoolRunner;

		info_t				fRequests;
		thread_t			fThreads;
};

#endif
