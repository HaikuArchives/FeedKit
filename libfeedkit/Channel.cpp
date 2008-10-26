#include "FeedKitConstants.h"
#include "Channel.h"
#include "Enclosure.h"
#include "Item.h"
#include "Feed.h"
#include "ItemSpecification.h"

#include <Message.h>

#include <openssl/sha.h>

#include <stdio.h>
#include <stdlib.h>
#include <parsedate.h>

//#pragma mark Namespace import

using namespace FeedKit;

//#pragma mark Constants

const type_code kTypeCode = 'tfkc';

//#pragma mark Constructor

Channel::Channel(void)
	: fFeed(NULL),
	fFeedUUID(""),
	fUUID(""),
	fLocalUUID(""),
	fTitle(""),
	fLink(""),
	fDescription(""),
	fLanguage(""),
	fCopyright(""),
	fWebmaster(""),
	fCategory(""),
	fGenerator(""),
	fDocs(""),
	fTTL(0),
	fPICSRating(""),
	fImageUUID(""),
	fImage(NULL) {
	
	fEnclosure.clear();
	
	time_t now = time(NULL);
	fPublishedDate = now;
	fLastBuildDate = now;
	
	fItem.clear();
};

Channel::Channel(const Channel &rhs)
	: BFlattenable(),
	fFeed(rhs.fFeed),
	fFeedUUID(rhs.fFeedUUID),
	fUUID(""),
	fLocalUUID(""),
	fTitle(rhs.fTitle),
	fLink(rhs.fLink),
	fDescription(rhs.fDescription),
	fLanguage(rhs.fLanguage),
	fCopyright(rhs.fCopyright),
	fEditor(rhs.fEditor),
	fWebmaster(rhs.fWebmaster),
	fPublishedDate(rhs.fPublishedDate),
	fLastBuildDate(rhs.fLastBuildDate),
	fCategory(rhs.fCategory),
	fGenerator(rhs.fGenerator),
	fDocs(rhs.fDocs),
	fTTL(rhs.fTTL),
	fPICSRating(rhs.fPICSRating),
	fImageUUID(""),
	fImage(NULL) {
	
	fImageUUID = rhs.fImageUUID;
	
	for (enclosure_list_t::const_iterator eIt = rhs.fEnclosure.begin(); eIt != rhs.fEnclosure.end(); eIt++) {
		fEnclosure.push_back(new Enclosure((*eIt)));
	};

	for (item_list_t::const_iterator iIt = rhs.fItem.begin(); iIt != rhs.fItem.end(); iIt++) {
		fItem.push_back(new Item((*iIt)));
	};
};

Channel::Channel(const Channel * const rhs, bool deep = true)
	: fFeed(rhs->fFeed),
	fFeedUUID(rhs->fFeedUUID),
	fUUID(""),
	fLocalUUID(""),
	fTitle(rhs->fTitle),
	fLink(rhs->fLink),
	fDescription(rhs->fDescription),
	fLanguage(rhs->fLanguage),
	fCopyright(rhs->fCopyright),
	fEditor(rhs->fEditor),
	fWebmaster(rhs->fWebmaster),
	fPublishedDate(rhs->fPublishedDate),
	fLastBuildDate(rhs->fLastBuildDate),
	fCategory(rhs->fCategory),
	fGenerator(rhs->fGenerator),
	fDocs(rhs->fDocs),
	fTTL(rhs->fTTL),
	fPICSRating(rhs->fPICSRating),
	fImageUUID(""),
	fImage(NULL) {
	
	fImageUUID = rhs->fImageUUID;
	
	for (enclosure_list_t::const_iterator eIt = rhs->fEnclosure.begin(); eIt != rhs->fEnclosure.end(); eIt++) {
		fEnclosure.push_back(new Enclosure((*eIt)));
	};

	if (deep) {				
		for (item_list_t::const_iterator iIt = rhs->fItem.begin(); iIt != rhs->fItem.end(); iIt++) {
			fItem.push_back(new Item((*iIt)));
		};
	};
};

Channel::~Channel(void) {
	fItem.clear();
	fEnclosure.clear();
};

//#pragma mark BFlattenable Hooks

status_t Channel::Flatten(void *buffer, ssize_t numBytes) const {
	BMessage flat;
	
	flat.AddString("uuid", fUUID);
	flat.AddString("localuuid", fLocalUUID);
	
	// Parent
	flat.AddString("feed", fFeedUUID);
	
	// Compulsory elements
	flat.AddString("title", fTitle);
	flat.AddString("link", fLink);
	flat.AddString("description", fDescription);
	
	// Optional elements
	flat.AddString("language", fLanguage);
	flat.AddString("copyright", fCopyright);
	flat.AddString("editor", fEditor);
	flat.AddString("webmaster", fWebmaster);
	flat.AddInt32("publisheddate", fPublishedDate);
	flat.AddInt32("lastbuilddate", fLastBuildDate);
	flat.AddString("category", fCategory);
	flat.AddString("generator", fGenerator);
	flat.AddString("docs", fDocs);
	flat.AddInt32("ttl", fTTL);
	flat.AddString("picsrating", fPICSRating);
	
	flat.AddString("image", fImageUUID);
	
	for (item_list_t::const_iterator iIt = fItem.begin(); iIt != fItem.end(); iIt++) {
		flat.AddFlat("item", (*iIt));
	};
	
	// Add the enclosures
	for (enclosure_list_t::const_iterator eIt = fEnclosure.begin(); eIt != fEnclosure.end(); eIt++) {
		flat.AddFlat("enclosure", (*eIt));
	};

	return flat.Flatten((char *)buffer, numBytes);
};

status_t Channel::Unflatten(type_code code, const void *buffer, ssize_t numBytes) {
	(void)code;
	(void)numBytes;

	BMessage flat;
	status_t ret = flat.Unflatten((char *)buffer);
	
	if (ret == B_OK) {
		if (flat.FindString("uuid", &fUUID) != B_OK) fUUID = "";
		if (flat.FindString("localuuid", &fLocalUUID) != B_OK) fLocalUUID = "";
	
		// Parent
		if (flat.FindString("feed", &fFeedUUID) != B_OK) ret = B_ERROR;
	
		// Compulsory elements
		if (flat.FindString("title", &fTitle) != B_OK) ret = B_ERROR;
		if (flat.FindString("link", &fLink) != B_OK) ret = B_ERROR;
		if (flat.FindString("description", &fDescription) != B_OK) ret = B_ERROR;
		
		time_t now = time(NULL);
	
		// Optional elements
		if (flat.FindString("language", &fLanguage) != B_OK) fLanguage = "";
		if (flat.FindString("copyright", &fCopyright) != B_OK) fCopyright = "";
		if (flat.FindString("editor", &fEditor) != B_OK) fEditor = "";
		if (flat.FindString("webmaster", &fWebmaster) != B_OK) fWebmaster = "";
		if (flat.FindInt32("publisheddate", (int32 *)&fPublishedDate) != B_OK) fPublishedDate = now;
		if (flat.FindInt32("lastbuilddate", (int32 *)&fLastBuildDate) != B_OK) fLastBuildDate = now;
		if (flat.FindString("category", &fCategory) != B_OK) fCategory = "";
		if (flat.FindString("generator", &fGenerator) != B_OK) fGenerator = "";
		if (flat.FindString("docs", &fDocs) != B_OK) fDocs = "";
		if (flat.FindInt32("ttl", &fTTL) != B_OK) fTTL = 0;
		if (flat.FindString("picsrating", &fPICSRating) != B_OK) fPICSRating = "";
				
		BString enclosure;
		if (flat.FindString("image", &enclosure) == B_OK) {
			fImageUUID = enclosure;
		};
		
		Item item;
		for (int32 i = 0; flat.FindFlat("item", i, &item) == B_OK; i++) {
			Item *copy = new Item(item);
			item = Item();

			// XXX		
			copy->ParentChannel(this);
			fItem.push_back(copy);
		}

		// Extract the enclosures
		Enclosure enc;
		for (int32 i = 0; flat.FindFlat("enclosure", i, &enc) == B_OK; i++) {
			Enclosure *copy = new Enclosure(enc);
			enc = Enclosure();
		
			fEnclosure.push_back(copy);
		};
	};
		
	return ret;
};

ssize_t Channel::FlattenedSize(void) const {
	BMessage flat;
	
	flat.AddString("uuid", fUUID);
	flat.AddString("localuuid", fLocalUUID);
	
	// Parent
	flat.AddString("feed", fFeedUUID);
	
	// Compulsory elements
	flat.AddString("title", fTitle);
	flat.AddString("link", fLink);
	flat.AddString("description", fDescription);
	
	// Optional elements
	flat.AddString("language", fLanguage);			
	flat.AddString("copyright", fCopyright);
	flat.AddString("editor", fEditor);
	flat.AddString("webmaster", fWebmaster);
	flat.AddInt32("publisheddate", fPublishedDate);
	flat.AddInt32("lastbuilddate", fLastBuildDate);
	flat.AddString("category", fCategory);
	flat.AddString("generator", fGenerator);
	flat.AddString("docs", fDocs);
	flat.AddInt32("ttl", fTTL);
	flat.AddString("picsrating", fPICSRating);

	flat.AddString("image", fImageUUID);
	
	for (item_list_t::const_iterator iIt = fItem.begin(); iIt != fItem.end(); iIt++) {
		flat.AddFlat("item", (*iIt));
	};

	// Add the enclosures	
	for (enclosure_list_t::const_iterator eIt = fEnclosure.begin(); eIt != fEnclosure.end(); eIt++) {
		flat.AddFlat("enclosure", (*eIt));
	}

	return flat.FlattenedSize();
};

bool Channel::IsFixedSize(void) const {
	return false;
};

type_code Channel::TypeCode(void) const {
	return kTypeCode;
};

bool Channel::AllowsTypeCode(type_code code) const {
	return code == kTypeCode;
};

//#pragma mark Compulsory Elements

const char *Channel::Title(void) const {
	return fTitle.String();
};

const char *Channel::Link(void) const {
	return fLink.String();
};

const char *Channel::Description(void) const {
	return fDescription.String();
};

//#pragma mark Optional Elements

const char *Channel::Language(void) const {
	return fLanguage.String();
};

const char *Channel::Copyright(void) const {
	return fEditor.String();
};

const char *Channel::Editor(void) const {
	return fEditor.String();
};

const char *Channel::Webmaster(void) const {
	return fWebmaster.String();
};

time_t Channel::PublishedDate(void) const {
	return fPublishedDate;
};

time_t Channel::BuildDate(void) const {
	return fLastBuildDate;
};

const char *Channel::Category(void) const {
	return fCategory.String();
};

const char *Channel::Generator(void) const {
	return fGenerator.String();
};

const char *Channel::Docs(void) const {
	return fDocs.String();
};

long Channel::TTL(void) const {
	return fTTL;
};

const char *Channel::PICSRating(void) const {
	return fPICSRating.String();
};

Enclosure *Channel::Image(void) const {
	return fImage;
};

uint32 Channel::ItemCount(void) const {
	return fItem.size();
};

Item *Channel::ItemAt(uint32 index) const {
	Item *item = NULL;
	if (index < ItemCount()) item = fItem[index];
	
	return item;
};

Item *Channel::ItemByUUID(const char *uuid) const {
	ItemList items = FindItems(new UUIDItemSpecification(uuid), true);
	Item *item = NULL;
	
	if (items.size() > 0) item = items[0];	

	return item;
};

uint32 Channel::EnclosureCount(void) const {
	return fEnclosure.size();
};

Enclosure *Channel::EnclosureAt(uint32 index) const {
	Enclosure *enclosure = NULL;
	if (index < EnclosureCount()) enclosure = fEnclosure[index];
	
	return enclosure;
};

ItemList Channel::FindItems(ItemSpecification *spec, bool autodelete) const {
	ItemList list;
	
	for (item_list_t::const_iterator iIt = fItem.begin(); iIt != fItem.end(); iIt++) {
		Item *item = (*iIt);
	
		if (spec->IsSatisfiedBy(item) == true) {
			list.push_back(item);
		};
	};
	
	if (autodelete) delete spec;
	
	return list;
};

EnclosureList Channel::FindEnclosures(EnclosureSpecification *spec, bool autodelete) const {
	EnclosureList list;
	
	for (enclosure_list_t::const_iterator eIt = fEnclosure.begin(); eIt != fEnclosure.end(); eIt++) {
		if (spec->IsSatisfiedBy(*eIt) == true) {
			list.push_back(*eIt);
		};
	};
	
	if (autodelete) delete spec;
	
	return list;
};

//#pragma mark Public

void Channel::ParentFeed(Feed *parent) {
	fFeed = parent;
	fFeedUUID = parent->UUID();
	
	fUUID = "";
	fLocalUUID = "";
};

Feed *Channel::ParentFeed(void) const {
	return fFeed;
};

bool Channel::Contains(Item *item) const {
	ItemList items = FindItems(new UUIDItemSpecification(item->UUID()), true);
	
	return (items.empty() == false);
};

//#pragma mark Public Set

void Channel::Title(const char *title) {
	fTitle = title;
};

void Channel::Link(const char *link) {
	fLink = link;
};

void Channel::Description(const char *description) {
	fDescription = description;
};

void Channel::Language(const char *language) {
	fLanguage = language;
};

void Channel::Copyright(const char *copyright) {
	fCopyright = copyright;
};

void Channel::Editor(const char *editor) {
	fEditor = editor;
};

void Channel::Webmaster(const char *webmaster) {
	fWebmaster = webmaster;
};

void Channel::PublishedDate(time_t published) {
	fPublishedDate = published;
};

void Channel::BuildDate(time_t build) {
	fLastBuildDate= build;
};

void Channel::Category(const char *category) {
	fCategory = category;
};

void Channel::Generator(const char *generator) {
	fGenerator = generator;
};

void Channel::Docs(const char *docs) {
	fDocs = docs;
};

void Channel::TTL(long ttl) {
	fTTL = ttl;
};

void Channel::PICSRating(const char *pics) {
	fPICSRating = pics;
};

void Channel::Image(Enclosure *image) {
	fImage = image;
	fImageUUID = image->UUID();
};

void Channel::AddItem(Item *item) {
	item->ParentChannel(this);
	fItem.push_back(item);
};

void Channel::AddEnclosure(Enclosure *enclosure) {
	fEnclosure.push_back(enclosure);
};

const char *Channel::UUID(void) const {
	if (fUUID.Length() == 0) {
		fUUID = fFeedUUID;
		fUUID << "_";
		fUUID << LocalUUID();
	};
	
	return fUUID.String();
};

const char *Channel::LocalUUID(void) const {
	if (fLocalUUID.Length() == 0) {
		BString localID = "";
		uchar hash[SHA_DIGEST_LENGTH];
		char buffer[2];

		memset(hash, '\0', SHA_DIGEST_LENGTH);
		memset(buffer, '\0', 2);

		localID << Link() << "#" << Title();
		
		SHA1((const uchar *)localID.String(), localID.Length(), hash);

		for (int32 i = 0; i < SHA_DIGEST_LENGTH; i++) {
			sprintf(buffer, "%02x", hash[i]);
			
			fLocalUUID << buffer;
		};
	};
	
	return fLocalUUID.String();
};

//#pragma mark Operands

bool Channel::operator == (const Channel &compare) const {
	return (strcmp(UUID(), compare.UUID()) == 0);
};

bool Channel::operator != (const Channel &compare) const {
	return (strcmp(UUID(), compare.UUID()) != 0);
};

//#pragma mark Protected

void Channel::UpdateUUID(void) {
	fUUID = "";
	fLocalUUID = "";
	
	UUID();
};

//#pragma mark Private
