#include "LocalEnclosureDownload.h"
#include "FeedServerConstants.h"
#include "Common/IMKitUtilities.h"

#include <stdio.h>

#include <support/TypeConstants.h>

#include <libfeedkit/Enclosure.h>

//#pragma mark Constants

const char *kAttrEnclosureUUID = "feedkit:enclosure_uuid";
const char *kAttrEnclosureSize = "feedkit:enclosure_size";
const char *kAttrEnclosureCancelled = "feedkit:enclosure_cancelled";

//#pragma mark Constructor

LocalEnclosureDownload::LocalEnclosureDownload(void) {
};

LocalEnclosureDownload::LocalEnclosureDownload(entry_ref ref)
	: fRef(ref) {
};

LocalEnclosureDownload::LocalEnclosureDownload(entry_ref ref, FeedKit::Enclosure *enclosure)
	: fRef(ref) {
	
	// Write attributes to disc
	BNode node(&ref);
	if (node.InitCheck() == B_OK) {
		int32 size = enclosure->Size();
		bool cancelled = (enclosure->State() == FeedKit::Cancelled);
	
		node.WriteAttr(kAttrEnclosureUUID, B_STRING_TYPE, 0, enclosure->UUID(), strlen(enclosure->UUID()));
		node.WriteAttr(kAttrEnclosureSize, B_INT32_TYPE, 0, reinterpret_cast<void *>(&size), sizeof(size));
		node.WriteAttr(kAttrEnclosureCancelled, B_BOOL_TYPE, 0, reinterpret_cast<void *>(&cancelled), sizeof(cancelled));
	};

	node.Unset();
};

LocalEnclosureDownload::~LocalEnclosureDownload(void) {
};

	
//#pragma mark Public

const char *LocalEnclosureDownload::EnclosureUUID(void) const {
	BString uuid = "";
	int32 length = B_ERROR;
	char *tempUUID = ReadAttribute(BNode(&fRef), kAttrEnclosureUUID, &length);

	if (tempUUID != NULL) uuid.SetTo(tempUUID, length);
	free(tempUUID);	
	
	return uuid.String();
};

bool LocalEnclosureDownload::Cancelled(void) const {
	bool cancelled = false;
	BNode node (&fRef);
	
	if (node.InitCheck() == B_OK) {
		node.ReadAttr(kAttrEnclosureCancelled, B_BOOL_TYPE, 0, reinterpret_cast<void *>(&cancelled), sizeof(cancelled));
	};

	return cancelled;
};

bool LocalEnclosureDownload::Complete(void) const {
	bool complete = false;
	int32 expected = B_ERROR;
	int32 current = B_ERROR;
	BNode node(&fRef);
	
	if (node.InitCheck() == B_OK) {
		off_t size = -1;

		node.ReadAttr(kAttrEnclosureSize, B_INT32_TYPE, 0, reinterpret_cast<void *>(&expected), sizeof(expected));
		node.GetSize(&size);
		
		current = (int32)size;
		complete = (current == expected);
	};
	
	node.Unset();
	
	return complete;
};

int32 LocalEnclosureDownload::ExpectedSize(void) const {
	int32 expectedSize = B_ERROR;
	BNode node(&fRef);
	
	if (node.InitCheck() == B_OK) {
		node.ReadAttr(kAttrEnclosureSize, B_INT32_TYPE, 0, reinterpret_cast<void *>(&expectedSize), sizeof(expectedSize));
	};
	node.Unset();

	return expectedSize;
};

int32 LocalEnclosureDownload::CurrentSize(void) const {
	int32 currentSize = B_ERROR;
	BNode node(&fRef);

	if (node.InitCheck() == B_OK) {
		off_t size = -1;
		node.GetSize(&size);
		
		currentSize = (int32)size;
	};
	node.Unset();
	
	return currentSize;
};

void LocalEnclosureDownload::CancelDownload(void) {
	BNode node(&fRef);

	if (node.InitCheck() == B_OK) {
		bool cancelled = true;
	
		node.WriteAttr(kAttrEnclosureCancelled, B_BOOL_TYPE, 0, reinterpret_cast<void *>(&cancelled), sizeof(cancelled));
	};	
};