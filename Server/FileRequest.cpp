#include "FileRequest.h"
#include "FileRequestHandler.h"

#include <String.h>

#include <curl/curlver.h>
#include <curl/curl.h>
#include <curl/easy.h>

#include <stdlib.h>

//#pragma mark RequestHandlerInfo

class RequestHandlerInfo {
	public:
							RequestHandlerInfo(FileRequestHandler *handler, void *data)
								: fHandler(handler),
								fData(data) {
							};
							~RequestHandlerInfo(void) {
								printf("~RequestHandlerInfo()\n");
								delete fHandler;
							};
		void				DownloadStarted(FileRequest *request) {
								fHandler->DownloadStarted(request, fData);
							};
		void				DownloadProgress(FileRequest *request, double downTotal,
								double downCurrent) {
								
								fHandler->DownloadProgress(request, fData, downTotal,
									downCurrent);
							};
	
	private:
		FileRequestHandler	*fHandler;
		void				*fData;
};


//#pragma mark Constants

const int32 kMaxMallocIOSize = 1024 * 1024 * 10;	// 10 MB

//#pragma mark Constructor

FileRequest::FileRequest(const char *url, entry_ref ref, BMessage progress, int32 offset)
	: fURL(url),
	fRef(ref),
	fProgress(progress),
	fOffset(offset) {

	fFile.SetTo(&ref, B_READ_WRITE | B_CREATE_FILE);

	Init();
};

FileRequest::FileRequest(const char *url, BMessage progress, int32 offset)
	: fURL(url),
	fProgress(progress),
	fOffset(offset) {
	
	const char *path = tmpnam(NULL);

	fFile.SetTo(path, B_READ_WRITE | B_CREATE_FILE);
	get_ref_for_path(path, &fRef);
	
	Init();
};

FileRequest::~FileRequest(void) {
	curl_easy_cleanup(fCurl);
	
	for (handlerinfo_t::iterator rIt = fHandler.begin(); rIt != fHandler.end(); rIt++) {
		delete (*rIt);
	};
};

//pragma mark Public

void FileRequest::AddHandler(FileRequestHandler *handler, void *data) {
	fHandler.push_back(new RequestHandlerInfo(handler, data));
};

status_t FileRequest::Start(void) {
	fStart = time(NULL);
	status_t result = B_ERROR;
	
	CURLcode curlResult = curl_easy_perform(fCurl);
	if (curlResult != CURLE_OK) {
		if (curlResult == CURLE_HTTP_RANGE_ERROR) {
			fprintf(stderr, "FileRequest::Start - %s couldn't be resumed, restarting\n", fURL.String());
			fOffset = 0;

			Init();
			result = Start();
		};
		if (curlResult == CURLE_ABORTED_BY_CALLBACK) result = DownloadCancelled;
	} else {
		result = B_OK;
	};
	
	fEnd = time(NULL);
	fComplete = true;
	
	return result;
};

void FileRequest::Cancel(void) {
	fCancelDownload = true;	
};

const char *FileRequest::URL(void) {
	return fURL.String();
};

int32 FileRequest::ExpectedSize(void) {
	return fExpectedSize;
};

float FileRequest::Speed(void) {
	time_t end = fEnd;
	int32 size = Size();
	
	if (fComplete == false) end = time(NULL);
	
	float duration = end - fStart;
	if (duration == 0) duration = 1;
	float speed = ((float)size) / duration;

	return speed;
};

time_t FileRequest::StartTime(void) {
	return fStart;
};

time_t FileRequest::EndTime(void) {
	return fEnd;
};

time_t FileRequest::Duration(void) {
	time_t end = fEnd;
	if (fComplete == false) end = time(NULL);

	return end - fStart;
};

int32 FileRequest::Size(void) {
	return fSize;
};

entry_ref FileRequest::Ref(void) {
	return fRef;
};

bool FileRequest::Completed(void) {
	return fComplete;
};

const char *FileRequest::MIME(void) {
	return fMIME.String();
};

BMessage FileRequest::ProgressMessage(void) {
	return fProgress;
};

//#pragma mark Private

void FileRequest::Init(void) {
	fStart = 0;
	fEnd = -1;
	fExpectedSize = 0;
	fComplete = false;
	fSize = 0;
	fMIME = "";
	fStartSent = false;
	fCancelDownload = false;
	
	fCurl = curl_easy_init();
	
	curl_easy_setopt(fCurl, CURLOPT_NOSIGNAL, TRUE);
	curl_easy_setopt(fCurl, CURLOPT_URL, fURL.String());
	curl_easy_setopt(fCurl, CURLOPT_WRITEFUNCTION, SaveData);
	curl_easy_setopt(fCurl, CURLOPT_WRITEDATA, this);
	curl_easy_setopt(fCurl, CURLOPT_HEADERFUNCTION, ParseHeader);
	curl_easy_setopt(fCurl, CURLOPT_HEADERDATA, this);
	curl_easy_setopt(fCurl, CURLOPT_FOLLOWLOCATION, true);
	curl_easy_setopt(fCurl, CURLOPT_MAXREDIRS, 10);
	curl_easy_setopt(fCurl, CURLOPT_NOPROGRESS, false);
	curl_easy_setopt(fCurl, CURLOPT_PROGRESSFUNCTION, Progress);
	curl_easy_setopt(fCurl, CURLOPT_PROGRESSDATA, this); 
	
	if (fOffset > 0) {
		curl_easy_setopt(fCurl, CURLOPT_RESUME_FROM, fOffset);
		fprintf(stderr, "FileRequest::Init() Will attempt to restart download at %i byte(s)\n", fOffset);
	}
};

//#pragma mark LibCurl Hooks

size_t FileRequest::ParseHeader(void *header, size_t size, size_t length, void *ptr) {
	FileRequest *us = reinterpret_cast<FileRequest *>(ptr);
		
	BString headerStr((char *)header, length);
	int32 colon = headerStr.IFindFirst(":");

	if (colon != B_ERROR) {
		BString value(headerStr);
		headerStr.Truncate(colon);
		value.Remove(0, colon + 2);

		if (headerStr.ICompare("Content-Length") == 0) {
			value.IReplaceAll("\r", "");
			value.IReplaceAll("\n", "");
			value.IReplaceAll(" ", "");
			us->fExpectedSize = atol(value.String());

		};
		if (headerStr.ICompare("Content-Type") == 0) {
			value.IReplaceAll("\r", "");
			value.IReplaceAll("\n", "");
			value.IReplaceAll(" ", "");
			
			us->fMIME = value;
		};
	};

	return size * length;
};

size_t FileRequest::SaveData(void *data, size_t size, size_t length, void *ptr) {
	FileRequest *us = reinterpret_cast<FileRequest *>(ptr);
	
	size_t write = us->fFile.Write(data, size * length);
	us->fSize += write;

	if (us->fStartSent == false) {
		for (handlerinfo_t::iterator rIt = us->fHandler.begin(); rIt != us->fHandler.end(); rIt++) {
			(*rIt)->DownloadStarted(us);
		};
		
		us->fStartSent = true;
	};

	return write;
};

int FileRequest::Progress(void *data, double downTotal, double downNow, double upTotal, double upNow) {
	FileRequest *us = reinterpret_cast<FileRequest *>(data);
	
	fprintf(stderr, "FileRequest::Progress([%p], %.2f, %.2f, %.2f, %.2f) called\n", data, downTotal, downNow, upTotal, upNow);

	for (handlerinfo_t::iterator rIt = us->fHandler.begin(); rIt != us->fHandler.end(); rIt++) {
		(*rIt)->DownloadProgress(us, downTotal, downNow);
	};
	

	return (us->fCancelDownload == true) ? 1 : 0;
};

