#include "ObjectUpdateFilter.h"
#include "ObjectSource.h"

#include <stdio.h>

//#pragma mark ObjectUpdateCommand

//#pragma mark Constrcutor

DelayedFlattenedObject::DelayedFlattenedObject(void)
	: BFlattenable(),
	fName(""),
	fObject(NULL) {
};

DelayedFlattenedObject::DelayedFlattenedObject(const char *name, BFlattenable *object)
	: BFlattenable(),
	fName(name),
	fObject(object) {
};
	
//#pragma mark Public
const char *DelayedFlattenedObject::Name(void) const {
	return fName.String();
};

BFlattenable *DelayedFlattenedObject::Object(void) const {
	return fObject;
};

//#pragma mark BFlattenable

status_t DelayedFlattenedObject::Flatten(void *buffer, ssize_t numBytes) const {
	BMessage flat;
	flat.AddString("name", fName);
	flat.AddPointer("object", fObject);
	
	return flat.Flatten((char *)buffer, numBytes);
};

status_t DelayedFlattenedObject::Unflatten(type_code code, const void *buffer, ssize_t numBytes) {
	BMessage flat;
	status_t ret = flat.Unflatten((char *)buffer);

	if (ret == B_OK) {
		if (flat.FindString("name", &fName) != B_OK) ret = B_ERROR;
		if (flat.FindPointer("object", reinterpret_cast<void **>(&fObject)) != B_OK) ret = B_ERROR;
	};
	
	return ret;
};

ssize_t DelayedFlattenedObject::FlattenedSize(void) const {
	BMessage flat;
	flat.AddString("name", fName);
	flat.AddPointer("object", fObject);

	return flat.FlattenedSize();
};
bool DelayedFlattenedObject::IsFixedSize(void) const {
	return false;
};

type_code DelayedFlattenedObject::TypeCode(void) const {
	return 'ouch';
};

bool DelayedFlattenedObject::AllowsTypeCode(type_code code) const {
	return code == 'ouch';
};

//#pragma mark ObjectUpdateFilter

//#pragma mark Constructor
ObjectUpdateFilter::ObjectUpdateFilter(ObjectSource *source)
	: BMessageFilter(B_ANY_DELIVERY, B_ANY_SOURCE),
	fSource(source) {
};

ObjectUpdateFilter::~ObjectUpdateFilter(void) {
};

//#pragma mark BMessageFilter Hooks


filter_result ObjectUpdateFilter::Filter(BMessage *msg, BHandler **/*target*/) {
	int32 what = msg->what;
	bool hadDelayed = FlattenDelayedObjects(msg);

	return B_DISPATCH_MESSAGE;
};

bool ObjectUpdateFilter::FlattenDelayedObjects(BMessage *msg) {
	bool hasDelayed = false;
	DelayedFlattenedObject delayed;

	for (int32 i = 0; msg->FindFlat("delayedupdate", i, &delayed) == B_OK; i++) {
		hasDelayed = true;
		BFlattenable *object = delayed.Object();
		
		if (object != NULL) msg->AddFlat(delayed.Name(), object);
	};

	if (hasDelayed == true) msg->RemoveName("delayedupdate");
	
	return hasDelayed;
};
