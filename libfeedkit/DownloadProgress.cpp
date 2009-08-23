#include "DownloadProgress.h"

#include <Message.h>

//#pragma mark Namespace import

using namespace FeedKit;

//#pragma mark Constants

const type_code kTypeCode = 'tfkd';

//#pragma mark Constructor

DownloadProgress::DownloadProgress(const char *mime, time_t start, int32 size, int32 downloaded,
	time_t end)
	: fMIME(mime),
	fStart(start),
	fEnd(end),
	fSize(size),
	fDownloaded(downloaded) {

	time_t now;
	time(&now);
	
	int32 seconds = now - fStart;
	fSpeed = (float)downloaded / (float)seconds;
		
//	if (fComplete == false) fEnd = CalculateETA();
};

DownloadProgress::DownloadProgress(const DownloadProgress *copy)
	: fMIME(copy->fMIME) {
}

DownloadProgress::DownloadProgress(void)
	: fMIME(""),
	fStart(-1),
	fEnd(-1),
	fSpeed(0),
	fSize(0),
	fDownloaded(0) {
};

DownloadProgress::~DownloadProgress(void) {
};

//#pragma mark BFlattenable Hooks

status_t DownloadProgress::Flatten(void *buffer, ssize_t numBytes) const {
	BMessage flat;
	flat.AddString("mime", fMIME);
	flat.AddInt32("start", fStart);
	flat.AddInt32("end", fEnd);
	flat.AddFloat("speed", fSpeed);
	flat.AddInt32("size", fSize);
	flat.AddInt32("downloaded", fDownloaded);
	
	return flat.Flatten((char *)buffer, numBytes);
};

status_t DownloadProgress::Unflatten(type_code /*code*/, const void *buffer, ssize_t /*numBytes*/) {
	BMessage flat;
	status_t ret = flat.Unflatten((char *)buffer);

	if (ret == B_OK) {
		if (flat.FindString("mime", &fMIME) != B_OK) ret = B_ERROR;
		if (flat.FindInt32("start", (int32 *)&fStart) != B_OK) ret = B_ERROR;
		if (flat.FindInt32("end", (int32 *)&fEnd) != B_OK) ret = B_ERROR;
		if (flat.FindFloat("speed", &fSpeed) != B_OK) ret = B_ERROR;
		if (flat.FindInt32("size", &fSize) != B_OK) ret = B_ERROR;
		if (flat.FindInt32("downloaded", &fDownloaded) != B_OK) ret = B_ERROR;
		
//		if ((ret == B_OK) && (fComplete == false)) fEnd = CalculateETA();
	};

	return ret;
};

ssize_t DownloadProgress::FlattenedSize(void) const {
	BMessage flat;
	flat.AddString("mime", fMIME);
	flat.AddInt32("start", fStart);
	flat.AddInt32("end", fEnd);
	flat.AddFloat("speed", fSpeed);
	flat.AddInt32("size", fSize);
	flat.AddInt32("downloaded", fDownloaded);

	return flat.FlattenedSize();
};

bool DownloadProgress::IsFixedSize(void) const {
	return false;
};

type_code DownloadProgress::TypeCode(void) const {
	return kTypeCode;
};

bool DownloadProgress::AllowsTypeCode(type_code code) const {
	return code == kTypeCode;
};

//#pragma mark Public

const char *DownloadProgress::MIME(void) const {
	return fMIME.String();
};

time_t DownloadProgress::StartTime(void) const {
	return fStart;
};

time_t DownloadProgress::EndTime(void) const {
	return fEnd;
};

int32 DownloadProgress::Duration(void) const {
	return fEnd - fStart;
};

float DownloadProgress::Speed(void) const {
	return fSpeed;
};

int32 DownloadProgress::Size(void) const {
	return fSize;
};

int32 DownloadProgress::AmountDownloaded(void) const {
	return fDownloaded;
};

float DownloadProgress::PercentageComplete(void) const {
	return (float)fDownloaded / (float)fSize;
};

//#pragma mark Private

time_t DownloadProgress::CalculateETA(void) {
	int32 remaining = fSize - fDownloaded;
	time_t seconds = (time_t)((float)remaining / fSpeed);
	return fStart + seconds;
};
