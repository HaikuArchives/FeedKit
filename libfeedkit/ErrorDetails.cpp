#include "ErrorDetails.h"

#include <Message.h>

using namespace FeedKit;

//#pragma mark Constants

const type_code kTypeCode = 'tfke';

//#pragma mark Constructor

ErrorDetails::ErrorDetails()
	: fCode(0),
	fMessage("") {
}

ErrorDetails::ErrorDetails(int32 code, const char *message)
	: fCode(code),
	fMessage(message) {
}

ErrorDetails::~ErrorDetails(void) {
}

//#pragma mark BFlattenable Hooks

status_t ErrorDetails::Flatten(void *buffer, ssize_t numBytes) const {
	BMessage flat;
	
	flat.AddInt32("code", fCode);
	flat.AddString("message", fMessage);

	return flat.Flatten((char *)buffer, numBytes);	
};

status_t ErrorDetails::Unflatten(type_code code, const void *buffer, ssize_t numBytes) {
	(void)code;
	(void)numBytes;
	
	BMessage flat;
	status_t result = flat.Unflatten((char *)buffer);
	
	if (result == B_OK) {
		if (flat.FindInt32("code", &fCode) != B_OK) result = B_ERROR;
		if (flat.FindString("message", &fMessage) != B_OK) result = B_ERROR;
	};
	
	return result;
};

ssize_t ErrorDetails::FlattenedSize(void) const {
	BMessage flat;
	
	flat.AddInt32("code", fCode);
	flat.AddString("message", fMessage);

	return flat.FlattenedSize();
};

bool ErrorDetails::IsFixedSize(void) const {
	return false;
};

type_code ErrorDetails::TypeCode(void) const {
	return kTypeCode;
};

bool ErrorDetails::AllowsTypeCode(type_code code) const {
	return code == kTypeCode;
};

//#pragma mark Public
		
int32 ErrorDetails::Code(void) {
	return fCode;
};

const char *ErrorDetails::Message(void) {
	return fMessage.String();
}
