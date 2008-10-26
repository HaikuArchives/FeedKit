#ifndef FILEREQUEST_H
#define FILEREQUEST_H

#include <Entry.h>
#include <File.h>
#include <String.h>
#include <Message.h>

#include <curl/curl.h>

#include <vector>

class FileRequestHandler;

enum FileRequestStatus {
	DownloadCancelled = CURLE_ABORTED_BY_CALLBACK,
};

class RequestHandlerInfo;

typedef vector<RequestHandlerInfo *> handlerinfo_t;

class FileRequest {
	public:
							FileRequest(const char *url, entry_ref ref, BMessage progress, int32 offset = 0);
							FileRequest(const char *url, BMessage progress = BMessage(), int32 offset = 0);
							~FileRequest(void);
		
		void				AddHandler(FileRequestHandler *handler, void *data);
				
		status_t			Start(void);
		void				Cancel(void);
		
		const char			*URL(void);
		int32				ExpectedSize(void);
		float				Speed(void);
		time_t				StartTime(void);
		time_t				EndTime(void);
		time_t				Duration(void);
		int32				Size(void);
		entry_ref			Ref(void);
		bool				Completed(void);
		const char			*MIME(void);
		
		BMessage			ProgressMessage(void);

	private:
		void				Init(void);
	
		// LibCurl hooks				
		static size_t		ParseHeader(void *header, size_t size, size_t length, void *ptr);
		static size_t		SaveData(void *data, size_t size, size_t length, void *ptr);
		static int			Progress(void *data, double downTotal, double doubleNow, double upTotal, double upNow);

		BString				fURL;
		entry_ref			fRef;
		BFile				fFile;
		CURL				*fCurl;
		time_t				fStart;
		time_t				fEnd;
		int32				fExpectedSize;
		int32				fSize;
		bool				fComplete;
		BString				fMIME;
		BMessage			fProgress;
		bool				fStartSent;
		int32				fOffset;
		bool				fCancelDownload;
		
		handlerinfo_t		fHandler;
};

#endif
