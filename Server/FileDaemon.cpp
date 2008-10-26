#include "FileDaemon.h"

#include <MessageRunner.h>

#include <stdio.h>
#include <algorithm>

#include "FileRequest.h"

//#pragma mark RequestInfo

class RequestInfo {
	public:
		RequestInfo(FileRequest *_request, int32 _priority, BMessenger *_target,
			BMessage *_start, BMessage *_finish, BMessage *_error, BMessage *_cancel) {
					
			request = _request;
			priority = _priority;
			queued = time(NULL);
			target = _target;
			start = _start;
			finish = _finish;
			error = _error;
			cancel = _cancel;
		};
	
		~RequestInfo(void) {
			delete target;
			delete start;
			delete finish;
			delete error;
			delete cancel;
		};
	
		bool operator < (RequestInfo *rhs) {
			bool moreImportant = priority > rhs->priority;
			
			// If the requests have equal importance, check if this is older
			if ((moreImportant == false) && (priority == rhs->priority)) {
				moreImportant = queued < rhs->queued;
			};

			return moreImportant;
		};
	
		FileRequest *request;
		int32 priority;
		time_t queued;
		BMessenger *target;
		BMessage *start;
		BMessage *finish;
		BMessage *error;
		BMessage *cancel;
};

//#pragma mark Constants

const bigtime_t kSecond = (bigtime_t)(1000 * 1000);
//const bigtime_t kSnoozeTime = 1 * kSecond;
//const bigtime_t kPoolTime = 5 * kSecond;
const bigtime_t kSnoozeTime = (bigtime_t)(0.5 * kSecond);
const bigtime_t kPoolTime = (bigtime_t)(0.5 * kSecond);
const int32 kPriorityLaunchThreshold = 100;

const int32 kGetRequest = 'fdgr';
const int32 kRequestComplete = 'fdgc';
const int32 kThreadComplete = 'fdtc';
const int32 kCheckPool = 'fdaw';

//#pragma mark Constructor

FileDaemon::FileDaemon(uint32 maxThreads)
	: BLooper("FileDaemon"),
	fMaxThreads(maxThreads) {
	
	Run();
	
	fThreadPoolRunner = new BMessageRunner(BMessenger(this), new BMessage(kCheckPool), kPoolTime);
};

FileDaemon::~FileDaemon(void) {
};

//#pragma mark BLooper Hooks

void FileDaemon::MessageReceived(BMessage *msg) {
	switch (msg->what) {
		case kCheckPool: {
			// A new thread will be launched if...
			//	- There's a request and no threads
			//	- The thread pool isn't full and the pool is working at 100% efficiency			
			if (fThreads.empty() == true) {
				LaunchThread();
				return;
			};
			
			uint32 threads = fThreads.size();
			if ((threads < fMaxThreads) && (fRequests.size() >= threads)) {
				LaunchThread();
				return;
			};
		} break;

		case kGetRequest: {
			thread_id thread = B_ERROR;
			if (msg->FindInt32("threadid", &thread) != B_OK) return;
			
			thread_t::iterator tIt = fThreads.find(thread);
			if (tIt == fThreads.end()) return;
			
			if (fRequests.empty() == true) {
				BMessage reply(B_REPLY);
				reply.AddBool("continue", fThreads.size() == 1);
				msg->SendReply(&reply);
				
				return;
			};

			RequestInfo *request = fRequests[0];
			fRequests.erase(fRequests.begin());
			BMessage reply(B_REPLY);
			reply.AddBool("continue", true);
			reply.AddPointer("requestinfo", request);
			msg->SendReply(&reply);
			
			tIt->second = request;
		} break;
	
		case kRequestComplete: {
			thread_id thread = B_ERROR;
			RequestInfo *info = NULL;
			
			if (msg->FindInt32("threadid", &thread) != B_OK) return;
			if (msg->FindPointer("requestinfo", reinterpret_cast<void **>(&info)) != B_OK) return;
			
			thread_t::iterator tIt = fThreads.find(thread);
			if (tIt != fThreads.end()) tIt->second = NULL;
			
			info_t::iterator iIt;
			for (iIt = fRequests.begin(); iIt != fRequests.end(); iIt++) {
				if ((*iIt) == info) {
					delete info;
					fRequests.erase(iIt);
				};
			}

		} break;
	
		case kThreadComplete: {
			thread_id thread = B_ERROR;
			if (msg->FindInt32("threadid", &thread) != B_OK) return;
			
			thread_t::iterator tIt = fThreads.find(thread);
			if (tIt != fThreads.end()) {
				delete tIt->second;
				fThreads.erase(tIt);
			};
		} break;
		default: {
			BLooper::MessageReceived(msg);
		};
	};
};

bool FileDaemon::QuitRequested(void) {
	return BLooper::QuitRequested();
};

//#pragma mark FileRequestHandler Hooks

void FileDaemon::DownloadStarted(FileRequest *request, void *data) {
	fprintf(stderr, "FileDaemon::DownloadStarted([%p], [%p]) called\n", request, data);

	// Send the commenced message
	RequestInfo *info = reinterpret_cast<RequestInfo* >(data);
	if ((info->target) && (info->start)) info->target->SendMessage(info->start);
};

//#pragma mark Public

status_t FileDaemon::AddRequest(FileRequest *request, int32 priority = 0, BMessenger *target = NULL,
	BMessage *start = NULL, BMessage *finish = NULL, BMessage *error = NULL, BMessage *cancel = NULL) {

	RequestInfo *info = new RequestInfo(request, priority, target, start, finish, error, cancel);
	request->AddHandler(this, info);
	
	fRequests.push_back(info);
	sort(fRequests.begin(), fRequests.end());

	if (priority >= kPriorityLaunchThreshold) BMessenger(this).SendMessage(new BMessage(kCheckPool));

	return B_OK;
};

request_t FileDaemon::ActiveRequests(void) {
	request_t active;
	thread_t::iterator tIt;

	for (tIt = fThreads.begin(); tIt != fThreads.end(); tIt++) {
		if (tIt->second != NULL) active.push_back(tIt->second->request);
	};
	
	return active;
};

status_t FileDaemon::CancelDownload(const char *url) {
	bool found = false;

	debugger("CancelDownload...");

	for (info_t::iterator rIt = fRequests.begin(); rIt != fRequests.end(); rIt++) {
		FileRequest *request = (*rIt)->request;

		if (strcmp(request->URL(), url) == 0) {
			request->Cancel();
			found = true;
			
			break;
		};
	};
	
	if (found == false) {
		for (thread_t::iterator tIt = fThreads.begin(); tIt != fThreads.end(); tIt++) {
			RequestInfo *info = tIt->second;
			
			if (info != NULL) {
				FileRequest *request = info->request;
				
				if (strcmp(request->URL(), url) == 0) {
					request->Cancel();
					found = true;
					
					break;
				};
			};
		};
	};
	
	return (found == true ? B_OK : B_ERROR);
};

//#pragma mark Private

void FileDaemon::LaunchThread() {
	thread_id thread = spawn_thread(WaitOnRequests, "File Downloader", B_LOW_PRIORITY, (void *)this);
	fprintf(stderr, "FileDaemon::LaunchThread(): Thread ID %i\n", thread);

	if (thread > B_ERROR) {
		resume_thread(thread);
		fThreads[thread] = NULL;
	};
};

int32 FileDaemon::WaitOnRequests(void *arg) {
	FileDaemon *us = reinterpret_cast<FileDaemon *>(arg);
	status_t err = B_ERROR;
	BMessenger msgr(us);
	bool work = true;
	thread_id thread = find_thread(NULL);

	while (work) {
		snooze(kSnoozeTime);

		BMessage getRequest(kGetRequest);
		getRequest.AddInt32("threadid", thread);
		BMessage reply;

		if (msgr.SendMessage(&getRequest, &reply) != B_OK) {
			work = false;
			continue;
		};

		RequestInfo *info = NULL;
		reply.FindBool("continue", &work);
		
		if (reply.FindPointer("requestinfo", reinterpret_cast<void **>(&info)) != B_OK) continue;
		if (info == NULL) continue;

		status_t result = info->request->Start();

		// Send the complete/ error message
		if (info->target) {
			fprintf(stderr, "FileDaemon::WaitOnRequests(): %s: %s (%i)\n", info->request->URL(),
				strerror(result), result);
		
			switch (result) {
				case B_OK: {
					if (info->finish) info->target->SendMessage(info->finish);
				} break;
				
				case DownloadCancelled: {
					if (info->cancel) info->target->SendMessage(info->cancel);
				} break;
				
				default: {
					if (info->error) info->target->SendMessage(info->error);
				} break;
			};
		};

		BMessage requestComplete(kRequestComplete);
		requestComplete.AddInt32("threadid", thread);
		requestComplete.AddPointer("requestinfo", &info);
		
		msgr.SendMessage(&requestComplete);
	};

	BMessage threadComplete(kThreadComplete);
	threadComplete.AddInt32("threadid", thread);
	msgr.SendMessage(&threadComplete);
	
	return B_OK;
};
