#include "Content.h"
#include "Item.h"

#include <Message.h>

#include <openssl/sha.h>

#include <stdlib.h>
#include <stdio.h>

//#pragma mark Namespace

using namespace FeedKit;

//#pragma mark Cnstants

const type_code kTypeCode = 'tfko';

//#pragma mark Constructor

Content::Content(void)
	: fItem(NULL),
	fItemUUID(""),
	fUUID(""),
	fLocalUUID(""),
	fMIMEType(""),
	fSummary("") {
};

Content::Content(const Content &rhs)
	: BFlattenable(),
	fItem(rhs.fItem),
	fItemUUID(rhs.fItemUUID),
	fUUID(rhs.fUUID),
	fLocalUUID(rhs.fLocalUUID),
	fMIMEType(rhs.fMIMEType),
	fSummary(rhs.fSummary),
	fText(rhs.fText) {
};

Content::Content(const Content * const rhs)
	: fItem(rhs->fItem),
	fItemUUID(rhs->fItemUUID),
	fUUID(rhs->fUUID),
	fLocalUUID(rhs->fLocalUUID),
	fMIMEType(rhs->fMIMEType),
	fSummary(rhs->fSummary),
	fText(rhs->fText) {
};

Content::~Content(void) {
};

//pragma mark BFlattenable Hooks
status_t Content::Flatten(void *buffer, ssize_t numBytes) const {
	BMessage flat;
	flat.AddString("item", fItemUUID);
	
	flat.AddString("uuid", fUUID);
	flat.AddString("localuuid", fLocalUUID);
	
	flat.AddString("mimetype", fMIMEType);
	flat.AddString("summary", fSummary);
	flat.AddString("text", fText);
	
	return flat.Flatten((char *)buffer, numBytes);
};

status_t Content::Unflatten(type_code /*code*/, const void *buffer, ssize_t /*numBytes*/) {
	BMessage flat;
	status_t ret = flat.Unflatten((char *)buffer);
	
	if (ret == B_OK) {
		if (flat.FindString("item", &fItemUUID) != B_OK) ret = B_ERROR;
		
		if (flat.FindString("uuid", &fUUID) != B_OK) ret = B_ERROR;
		if (flat.FindString("localuuid", &fLocalUUID) != B_OK) ret = B_ERROR;
		
		if (flat.FindString("mimetype", &fMIMEType) != B_OK) fMIMEType = "";
		if (flat.FindString("summary", &fSummary) != B_OK) fSummary = "";
		if (flat.FindString("text", &fText) != B_OK) fText = "";
		 
	};
	
	return ret;
};

ssize_t Content::FlattenedSize(void) const {
	BMessage flat;
	flat.AddString("item", fItemUUID);
	
	flat.AddString("uuid", fUUID);
	flat.AddString("localuuid", fLocalUUID);
	
	flat.AddString("mimetype", fMIMEType);
	flat.AddString("summary", fSummary);
	flat.AddString("text", fText);
	
	return flat.FlattenedSize();
};

bool Content::IsFixedSize(void) const {
	return false;
};

type_code Content::TypeCode(void) const {
	return kTypeCode;
};

bool Content::AllowsTypeCode(type_code code) const {
	return (kTypeCode == code);
};
	
//#pragma mark Public
void Content::ParentItem(Item *item) {
	fItemUUID = item->UUID();
	fItem = item;
	
	fUUID = "";
	fLocalUUID = "";
};

Item *Content::ParentItem(void) const {
	return fItem;
};

const char *Content::MIMEType(void) const {
	return fMIMEType.String();
};

const char *Content::Summary(void) const {
	return fSummary.String();
};

const char *Content::Text(void) const {
	return fText.String();
};
		
void Content::MIMEType(const char *type) {
	fMIMEType = type;
};

void Content::Summary(const char *summary) {
	fSummary = summary;
};

void Content::Text(const char *text) {
	fText = text;
};

const char *Content::UUID(void) const {
	if (fUUID.Length() == 0) {
		fUUID = fItemUUID;
		fUUID << "_";
		fUUID << LocalUUID();
	};
	
	return fUUID.String();
};

const char *Content::LocalUUID(void) const {
	if (fLocalUUID.Length() == 0) {
		BString localID = "";
		uchar hash[SHA_DIGEST_LENGTH];
		char buffer[2];

		memset(hash, '\0', SHA_DIGEST_LENGTH);
		memset(buffer, '\0', 2);

		localID << MIMEType() << "-" << Summary() << "#" << Text();
		
		SHA1((const uchar *)localID.String(), localID.Length(), hash);

		for (int32 i = 0; i < SHA_DIGEST_LENGTH; i++) {
			sprintf(buffer, "%02x", hash[i]);
			
			fLocalUUID << buffer;
		};
	};
	
	return fLocalUUID.String();
};

//#pragma mark Protected

void Content::UpdateUUID(void) {
	fLocalUUID = "";
	fUUID = "";
	
	UUID();
};

