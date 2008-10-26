#include "Enclosure.h"
#include "DownloadProgress.h"

#include <Message.h>

#include <openssl/sha.h>

#include <stdio.h>

//#pragma mark Namespace import

using namespace FeedKit;

//#pragma mark Constants

const type_code kTypeCode = 'tfke';

//#pragma mark Constructor

Enclosure::Enclosure(void)
	: fURL(""),
	fMIME(""),
	fDescription(""),
	fSize(-1),
	fState(None),
	fProgress(NULL),
	fLocalUUID("") {
};

Enclosure::Enclosure(const Enclosure &rhs)
	: BFlattenable(), 
	fURL(rhs.fURL),
	fMIME(rhs.fMIME), 
	fDescription(rhs.fDescription),
	fSize(rhs.fSize),
	fState(rhs.fState),
	fProgress(NULL),
	fLocalUUID("") {

	if (rhs.fProgress != NULL) {
		fProgress = new DownloadProgress(rhs.fProgress);
	};
	fLocalRef = rhs.fLocalRef;
};

Enclosure::Enclosure(const Enclosure * const rhs)
	: fURL(rhs->fURL),
	fMIME(rhs->fMIME),
	fDescription(rhs->fDescription),
	fSize(rhs->fSize),
	fState(rhs->fState),
	fProgress(NULL),
	fLocalUUID("") {

	if (rhs->fProgress != NULL) {
		fProgress = new DownloadProgress(rhs->fProgress);
	}	
	fLocalRef = rhs->fLocalRef;
};

Enclosure::Enclosure(const char *url, const char *mime = NULL, const char *description = NULL, int32 size = -1)
	: fURL(url),
	fMIME(""),
	fDescription(""),
	fSize(size),
	fState(None),
	fProgress(NULL),
	fLocalUUID("") {

	if (mime != NULL) fMIME = mime;
	if (description != NULL) fDescription = description;
};

Enclosure::~Enclosure(void) {
	if (fProgress) delete fProgress;
};

//#pragma mark BFlattenable Hooks

status_t Enclosure::Flatten(void *buffer, ssize_t numBytes) const {
	BMessage flat;
	flat.AddString("url", fURL);
	flat.AddString("mime", fMIME);
	flat.AddString("description", fDescription);
	flat.AddInt32("size", fSize);
	flat.AddInt32("state", fState);
	if (fProgress) flat.AddFlat("progress", fProgress);
	flat.AddRef("localRef", &fLocalRef);
	
	return flat.Flatten((char *)buffer, numBytes);
};

status_t Enclosure::Unflatten(type_code /*code*/, const void *buffer, ssize_t /*numBytes*/) {
	BMessage flat;
	status_t ret = flat.Unflatten((char *)buffer);
	fProgress = NULL;
	
	if (ret == B_OK) {
		// Compulsory
		if (flat.FindString("url", &fURL) != B_OK) ret = B_ERROR;

		// Optional
		if (flat.FindString("mime", &fMIME) != B_OK) fMIME = "";
		if (flat.FindString("description", &fDescription) != B_OK) fDescription = "";
		if (flat.FindInt32("size", &fSize) != B_OK) fSize = -1;
		
		if (flat.FindInt32("state", reinterpret_cast<int32 *>(&fState)) != B_OK) fState = Error;
		flat.FindRef("localRef", &fLocalRef);

		DownloadProgress progress;
		if (flat.FindFlat("progress", &progress) == B_OK) {
			fProgress = new DownloadProgress(progress);
		};
	};
	
	return ret;
};

ssize_t Enclosure::FlattenedSize(void) const {
	BMessage flat;
	flat.AddString("url", fURL);
	flat.AddString("mime", fMIME);
	flat.AddString("description", fDescription);
	flat.AddInt32("size", fSize);
	flat.AddInt32("state", fState);
	if (fProgress) flat.AddFlat("progress", fProgress);
	flat.AddRef("localRef", &fLocalRef);

	return flat.FlattenedSize();
};

bool Enclosure::IsFixedSize(void) const {
	return false;
};

type_code Enclosure::TypeCode(void) const {
	return kTypeCode;
};

bool Enclosure::AllowsTypeCode(type_code code) const {
	return code == kTypeCode;
};

//#pragma mark Public

const char *Enclosure::URL(void) const {
	return fURL.String();
};

const char *Enclosure::MIME(void) const {
	return fMIME.String();
};

const char *Enclosure::Description(void) const {
	return fDescription.String();
};

int32 Enclosure::Size(void) const {
	return fSize;
};

//#pragma mark Download Information

EnclosureState Enclosure::State(void) const {
	return fState;
};

const DownloadProgress *Enclosure::Progress(void) const {
	return fProgress;
};

entry_ref Enclosure::LocalRef(void) const {
	return fLocalRef;
};

const char *Enclosure::UUID(void) const {
	return LocalUUID();
};

const char *Enclosure::LocalUUID(void) const {
	if (fLocalUUID.Length() == 0) {
		BString localID = "";
		uchar hash[SHA_DIGEST_LENGTH];
		char buffer[2];
		
		localID << fURL << "#" << fMIME << "#" << fDescription << "#" << fSize;

		SHA1((const uchar *)localID.String(), localID.Length(), hash);

		for (int32 i = 0; i < SHA_DIGEST_LENGTH; i++) {
			sprintf(buffer, "%02x", hash[i]);
			
			fLocalUUID << buffer;
		};
	};
	
	return fLocalUUID.String();
};

//#pragma mark Operands

bool Enclosure::operator == (const Enclosure &compare) const {
	return (strcmp(UUID(), compare.UUID()) == 0);
};

bool Enclosure::operator != (const Enclosure &compare) const {
	return (strcmp(UUID(), compare.UUID()) != 0);
};

//#pragma mark Protected

void Enclosure::SetLocalRef(entry_ref ref) {
	fLocalRef = ref;
};

void Enclosure::SetState(EnclosureState state) {
	fState = state;
};

void Enclosure::SetProgress(DownloadProgress *progress) {
//	Deleting the old Progress causes a crash on resumed downloads
	delete fProgress;
	fProgress = NULL;
	
//	if (progress != NULL) {
//		fProgress = new DownloadProgress(progress);
//		fprintf(stderr, "Enclosure::SetProgress([%p]) vs [%p])\n", progress, fProgress);
//	};
};