#include "Item.h"

#include "Content.h"
#include "Enclosure.h"
#include "Feed.h"

#include <File.h>
#include <Message.h>
#include <OS.h>

#include <parsedate.h>

#include <openssl/sha.h>
#include <stdio.h>

//#pragma mark Namespace import

using namespace FeedKit;

//#pragma mark Constants

const type_code kTypeCode = 'tfki';

//#pragma mark Constructor

Item::Item(void)
	: fChannel(NULL),
	fChannelUUID(""),
	fUUID(""),
	fLocalUUID(""),
	fPermaLinkGUID(false),
	fGUID(""),
	fAuthor(""),
	fTitle(""),
	fLink(""),
	fCategory(""),
	fComments(""),
	fSourceURL(""),
	fSourceTitle(""),
	fRead(false),
	fCurrent(false),
	fNew(false) {
	
	fDate = time(NULL);
};

Item::Item(const Item &rhs)
	: BFlattenable(),
	fChannel(rhs.fChannel),
	fChannelUUID(rhs.fChannelUUID),
	fUUID(""),
	fLocalUUID(""),
	fPermaLinkGUID(rhs.fPermaLinkGUID),
	fGUID(rhs.fGUID),
	fAuthor(rhs.fAuthor),
	fTitle(rhs.fTitle),
	fLink(rhs.fLink),
	fCategory(rhs.fCategory),
	fComments(rhs.fComments),
	fDate(rhs.fDate),
	fSourceURL(rhs.fSourceURL),
	fSourceTitle(rhs.fSourceTitle),
	fRead(rhs.fRead),
	fCurrent(rhs.fCurrent),
	fNew(rhs.fNew) {

	for (enclosure_list_t::const_iterator eIt = rhs.fEnclosure.begin(); eIt != rhs.fEnclosure.end(); eIt++) {
		fEnclosure.push_back(new Enclosure((*eIt)));
	};
	
	for (content_list_t::const_iterator cIt = rhs.fContent.begin(); cIt != rhs.fContent.end(); cIt++) {
		fContent.push_back(new Content((*cIt)));
	};
};

Item::Item(const Item * const rhs)
	: fChannel(rhs->fChannel),
	fChannelUUID(rhs->fChannelUUID),
	fUUID(""),
	fLocalUUID(""),
	fPermaLinkGUID(rhs->fPermaLinkGUID),
	fGUID(rhs->fGUID),
	fAuthor(rhs->fAuthor),
	fTitle(rhs->fTitle),
	fLink(rhs->fLink),
	fCategory(rhs->fCategory),
	fComments(rhs->fComments),
	fDate(rhs->fDate),
	fSourceURL(rhs->fSourceURL),
	fSourceTitle(rhs->fSourceTitle),
	fRead(rhs->fRead),
	fCurrent(rhs->fCurrent),
	fNew(rhs->fNew) {

	for (enclosure_list_t::const_iterator eIt = rhs->fEnclosure.begin(); eIt != rhs->fEnclosure.end(); eIt++) {
		fEnclosure.push_back(new Enclosure((*eIt)));
	};
	
	for (content_list_t::const_iterator cIt = rhs->fContent.begin(); cIt != rhs->fContent.end(); cIt++) {
		fContent.push_back(new Content((*cIt)));
	};
};

Item::~Item(void) {
	fEnclosure.clear();
};

//#pragma mark BFlattenable Hooks

status_t Item::Flatten(void *buffer, ssize_t numBytes) const {
	BMessage flat;

	flat.AddString("uuid", fUUID);
	flat.AddString("localuuid", fLocalUUID);

	flat.AddString("channel", fChannelUUID);

	flat.AddBool("permalinkguid", fPermaLinkGUID);
	flat.AddString("guid", fGUID);
	flat.AddString("author", fAuthor);
	flat.AddString("title", fTitle);
	flat.AddString("link", fLink);
	flat.AddString("category", fCategory);
	flat.AddString("comments", fComments);
	flat.AddInt32("date", fDate);
	flat.AddString("sourceurl", fSourceURL);
	flat.AddString("sourcetitle", fSourceTitle);
	flat.AddBool("read", fRead);
	flat.AddBool("current", fCurrent);
	flat.AddBool("new", fNew);

	for (enclosure_list_t::const_iterator eIt = fEnclosure.begin(); eIt != fEnclosure.end(); eIt++) {
		flat.AddFlat("enclosure", (*eIt));
	}

	for (content_list_t::const_iterator cIt = fContent.begin(); cIt != fContent.end(); cIt++) {
		flat.AddFlat("content", (*cIt));
	};

	return flat.Flatten((char *)buffer, numBytes);
};

status_t Item::Unflatten(type_code code, const void *buffer, ssize_t numBytes) {
	(void)code;
	(void)numBytes;

	BMessage flat;
	status_t ret = flat.Unflatten((char *)buffer);

	if (ret == B_OK) {
		if (flat.FindString("uuid", &fUUID) != B_OK) fUUID = "";
		if (flat.FindString("localuuid", &fLocalUUID) != B_OK) fLocalUUID = "";
	
		if (flat.FindString("channel", &fChannelUUID) != B_OK) ret = B_ERROR;
	
		if (flat.FindBool("permalinkguid", &fPermaLinkGUID) != B_OK) ret = B_ERROR;
		if (flat.FindString("guid", &fGUID) != B_OK) ret = B_ERROR;
		if (flat.FindString("author", &fAuthor) != B_OK) ret = B_ERROR;
		if (flat.FindString("title", &fTitle) != B_OK) ret = B_ERROR;
		if (flat.FindString("link", &fLink) != B_OK) ret = B_ERROR;
		if (flat.FindString("category", &fCategory) != B_OK) ret = B_ERROR;
		if (flat.FindString("comments", &fComments) != B_OK) ret = B_ERROR;
		if (flat.FindInt32("date", &fDate) != B_OK) ret = B_ERROR;
		if (flat.FindString("sourceurl", &fSourceURL) != B_OK) ret = B_ERROR;
		if (flat.FindString("sourcetitle", &fSourceTitle) != B_OK) ret = B_ERROR;
		if (flat.FindBool("read", &fRead) != B_OK) ret = B_ERROR;
		if (flat.FindBool("current", &fCurrent) != B_OK) ret = B_ERROR;
		if (flat.FindBool("new", &fNew) != B_OK) ret = B_ERROR;

		Enclosure enclosure;
		for (int32 i = 0; flat.FindFlat("enclosure", i, &enclosure) == B_OK; i++) {
			Enclosure *copy = new Enclosure(enclosure);
			enclosure = Enclosure();
			
			fEnclosure.push_back(copy);
		};
		
		Content content;
		for (int32 i = 0; flat.FindFlat("content", i, &content) == B_OK; i++) {
			Content *copy = new Content(content);
			content = Content();
			
			// XXX
			copy->ParentItem(this);
			fContent.push_back(copy);
		};
	};
	
	return ret;
};

ssize_t Item::FlattenedSize(void) const {
	BMessage flat;
	
	flat.AddString("uuid", fUUID);
	flat.AddString("localuuid", fLocalUUID);

	flat.AddString("channel", fChannelUUID);

	flat.AddBool("permalinkguid", fPermaLinkGUID);
	flat.AddString("guid", fGUID);
	flat.AddString("author", fAuthor);
	flat.AddString("title", fTitle);
	flat.AddString("link", fLink);
	flat.AddString("category", fCategory);
	flat.AddString("comments", fComments);
	flat.AddInt32("date", fDate);
	flat.AddString("sourceurl", fSourceURL);
	flat.AddString("sourcetitle", fSourceTitle);
	flat.AddBool("read", fRead);
	flat.AddBool("current", fCurrent);
	flat.AddBool("new", fNew);

	for (enclosure_list_t::const_iterator eIt = fEnclosure.begin(); eIt != fEnclosure.end(); eIt++) {
		flat.AddFlat("enclosure", (*eIt));
	};

	for (content_list_t::const_iterator cIt = fContent.begin(); cIt != fContent.end(); cIt++) {
		flat.AddFlat("content", (*cIt));
	};

	return flat.FlattenedSize();
};

bool Item::IsFixedSize(void) const {
	return false;
};

type_code Item::TypeCode(void) const {
	return kTypeCode;
};

bool Item::AllowsTypeCode(type_code code) const {
	return code == kTypeCode;
};

//#pragma mark Public

void Item::ParentChannel(Channel *channel) {
	fChannel = channel;
	fChannelUUID = channel->UUID();
	
	fUUID = "";
	fLocalUUID = "";
};

Channel *Item::ParentChannel(void) const {
	return fChannel;
};

bool Item::IsGUIDPermaLink(void) const {
	return fPermaLinkGUID;
};

const char *Item::GUID(void) const {
	return fGUID.String();
};

const char *Item::Author(void) const {
	return fAuthor.String();
};

const char *Item::Title(void) const {
	return fTitle.String();
};

const char *Item::Link(void) const {
	return fLink.String();
};

const char *Item::Category(void) const {
	return fCategory.String();
};

const char *Item::Comments(void) const {
	return fComments.String();
};

time_t Item::Date(void) const {
	return fDate;
};

const char *Item::SourceURL(void) const {
	return fSourceURL.String();
};

const char *Item::SourceTitle(void) const {
	return fSourceTitle.String();
};

void Item::IsGUIDPermaLink(bool permalink) {
	fPermaLinkGUID = permalink;
};

void Item::GUID(const char *guid) {
	fGUID = guid;
};

void Item::Author(const char *author) {
	fAuthor = author;
};

void Item::Title(const char *title) {
	fTitle = title;
};

void Item::Link(const char *link) {
	fLink = link;
};

void Item::Category(const char *category) {
	fCategory = category;
};

void Item::Comments(const char *comments) {
	fComments = comments;
};

void Item::Date(time_t date) {
	fDate = date;
};

void Item::SourceURL(const char *url) {
	fSourceURL = url;
};

void Item::SourceTitle(const char *title) {
	fSourceTitle = title;
};

uint32 Item::EnclosureCount(void) const {
	return fEnclosure.size();
};

Enclosure *Item::EnclosureAt(uint32 index) const {
	Enclosure *enclosure = NULL;
	if (index < EnclosureCount()) enclosure = fEnclosure[index];
	
	return enclosure;
};

EnclosureList Item::FindEnclosures(EnclosureSpecification *spec, bool autodelete) const {
	EnclosureList list;
	
	for (enclosure_list_t::const_iterator eIt = fEnclosure.begin(); eIt != fEnclosure.end(); eIt++) {
		if (spec->IsSatisfiedBy(*eIt) == true) {
			list.push_back(*eIt);
		};
	};
	
	if (autodelete) delete spec;
	
	return list;
};

Enclosure *Item::EnclosureByUUID(const char *uuid) const {
	Enclosure *enclosure = NULL;
	
	for (uint32 i = 0; i < fEnclosure.size(); i++) {
		if (strcmp(fEnclosure[i]->UUID(), uuid) == 0) {
			enclosure = fEnclosure[i];
			break;
		};
	};
	
	return enclosure;
}

void Item::AddEnclosure(Enclosure *enclosure) {
	fEnclosure.push_back(enclosure);
};

uint32 Item::ContentCount(void) const {
	return fContent.size();
};

Content *Item::ContentAt(uint32 index) const {
	Content *content = NULL;
	if (index < ContentCount()) content = fContent[index];
	
	return content;
};

ContentList Item::FindContent(ContentSpecification *spec, bool autodelete) const {
	ContentList list;
	
	for (content_list_t::const_iterator cIt = fContent.begin(); cIt != fContent.end(); cIt++) {
		if (spec->IsSatisfiedBy(*cIt) == true) {
			list.push_back(*cIt);
		};
	};
	
	if (autodelete) delete spec;
	
	return list;
};

void Item::AddContent(Content *content) {
	fContent.push_back(content);
	content->ParentItem(this);
};

bool Item::Read(void) const {
	return fRead;
};

bool Item::Current(void) const {
	return fCurrent;
};

bool Item::New(void) const {
	return fNew;
};

const char *Item::UUID(void) const{
	if (fUUID.Length() == 0) {
		fUUID = fChannelUUID;
		fUUID << "_";
		fUUID << LocalUUID();
	};
		
	return fUUID.String();
};

const char *Item::LocalUUID(void) const {
	if (fLocalUUID.Length() == 0) {
		BString localID = "";
		uchar hash[SHA_DIGEST_LENGTH];
		char buffer[2];
		
		localID << fAuthor << "/" << fTitle << "#";
		if (fGUID.Length() > 0) {
			localID << fGUID;
		} else {
			localID << fLink;
		};
		
		SHA1((const uchar *)localID.String(), localID.Length(), hash);

		for (int32 i = 0; i < SHA_DIGEST_LENGTH; i++) {
			sprintf(buffer, "%02x", hash[i]);
			
			fLocalUUID << buffer;
		};

	};
	
	return fLocalUUID.String();
};

//#pragma mark Operands

bool Item::operator == (const Item &compare) const {
	return (strcmp(UUID(), compare.UUID()) == 0);
};

bool Item::operator != (const Item &compare) const {
	return (strcmp(UUID(), compare.UUID()) != 0);
};

//#pragma mark Protected

void Item::SetRead(bool read) {
	fRead = read;
};

void Item::SetCurrent(bool current) {
	fCurrent = current;
};

void Item::SetNew(bool isNew) {
	fNew = isNew;
};

void Item::UpdateUUID(void) {
	fUUID = "";
	fLocalUUID = "";
	
	UUID();
};

//#pragma mark Private

