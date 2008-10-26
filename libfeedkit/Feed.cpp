#include "Feed.h"
#include "Channel.h"

#include <Message.h>

#include <openssl/sha.h>

#include <stdio.h>

//#pragma mark Namespace import

using namespace FeedKit;

//#pragma mark Constants

const type_code kTypeCode = 'tfkf';

//#pragma mark Constructor

Feed::Feed(const char *url, const char *name = "")
	: fURL(url),
	fName(name),
	fSubscribed(true),
	fUUID("") {
	
	fChannel.clear();
};

Feed::Feed(const Feed &rhs)
	: BFlattenable(),
	fURL(rhs.fURL),
	fName(rhs.fName),
	fSubscribed(rhs.fSubscribed),
	fUUID("") {
	
	fChannel.clear();
	
	for (channel_list_t::const_iterator cIt = rhs.fChannel.begin(); cIt != rhs.fChannel.end(); cIt++) {
		fChannel.push_back(new Channel((*cIt)));
	}
};

Feed::Feed(const Feed * const rhs)
	: fURL(rhs->fURL),
	fName(rhs->fName),
	fSubscribed(rhs->fSubscribed),
	fUUID("") {
	
	fChannel.clear();
	
	for (channel_list_t::const_iterator cIt = rhs->fChannel.begin(); cIt != rhs->fChannel.end(); cIt++) {
		fChannel.push_back(new Channel((*cIt)));
	}
};

Feed::Feed(void)
	: fURL(""),
	fName(""),
	fSubscribed(false),
	fUUID("") {

	fChannel.clear();
};

Feed::~Feed(void) {
	fChannel.clear();
};

//#pragma mark BFlattenable Hooks

status_t Feed::Flatten(void *buffer, ssize_t numBytes) const {
	BMessage flat;
	flat.AddString("url", fURL);
	flat.AddString("name", fName);
	flat.AddBool("subscribed", fSubscribed);
	flat.AddString("uuid", fUUID);

	for (channel_list_t::const_iterator cIt = fChannel.begin(); cIt != fChannel.end(); cIt++) {
		flat.AddFlat("channel", (*cIt));
	}

	return flat.Flatten((char *)buffer, numBytes);
};

status_t Feed::Unflatten(type_code /*code*/, const void *buffer, ssize_t /*numBytes*/) {
	BMessage flat;
	status_t ret = flat.Unflatten((char *)buffer);

	if (ret == B_OK) {
		if (flat.FindString("url", &fURL) != B_OK) ret = B_ERROR;
		if (flat.FindString("name", &fName) != B_OK) fName = "";
		if (flat.FindBool("subscribed", &fSubscribed) != B_OK) fSubscribed = true;
		if (flat.FindString("uuid", &fUUID) != B_OK) fUUID = "";
		
		Channel channel;
		for (int32 i = 0; flat.FindFlat("channel", i, &channel) == B_OK; i++) {
			Channel *copy = new Channel(channel);
			channel = Channel();
		
			// XXX
			copy->ParentFeed(this);
			fChannel.push_back(copy);
		}
	};

	return ret;
};

ssize_t Feed::FlattenedSize(void) const {
	BMessage flat;
	flat.AddString("url", fURL);
	flat.AddString("name", fName);
	flat.AddBool("subscribed", fSubscribed);
	flat.AddString("uuid", fUUID);

	for (channel_list_t::const_iterator cIt = fChannel.begin(); cIt != fChannel.end(); cIt++) {
		flat.AddFlat("channel", (*cIt));
	}

	return flat.FlattenedSize();
};

bool Feed::IsFixedSize(void) const {
	return false;
};

type_code Feed::TypeCode(void) const {
	return kTypeCode;
};

bool Feed::AllowsTypeCode(type_code code) const {
	return kTypeCode == code;
};

		
//#pragma mark Public

const char *Feed::URL(void) const {
	return fURL.String();
};

const char *Feed::Name(void) const {
	return fName.String();
};

const char *Feed::DisplayName(void) const {
	return (fName.Length() == 0) ? fURL.String() : fName.String();
};

bool Feed::Subscribed(void) const {
	return fSubscribed;
};

//#pragma mark Tree traversal

uint32 Feed::ChannelCount(void) const {
	return fChannel.size();
};

Channel *Feed::ChannelAt(uint32 index) const{
	Channel *channel = NULL;
	if (index < fChannel.size()) {
		channel = fChannel[index];
	};

	return channel;
};

ChannelList Feed::FindChannels(ChannelSpecification *spec, bool autodelete) const {
	ChannelList list;
	
	for (channel_list_t::const_iterator cIt = fChannel.begin(); cIt != fChannel.end(); cIt++) {
		if (spec->IsSatisfiedBy(*cIt) == true) {
			list.push_back(*cIt);
		};
	};
	
	if (autodelete) delete spec;
	
	return list;

};

Channel *Feed::ChannelByUUID(const char *uuid) const {
	Channel *channel = NULL;

	for (uint32 i = 0; i < fChannel.size(); i++) {
		if (strcmp(fChannel[i]->UUID(), uuid) == 0) {
			channel = fChannel[i];
			break;
		}
	};
	
	return channel;
};

void Feed::AddChannel(Channel *channel) {
	channel->ParentFeed(this);
	
	fChannel.push_back(channel);
};

bool Feed::Contains(Channel *channel) const {
	return (FindChannel(channel) >= 0);
};

Channel *Feed::EquivalentChannel(Channel *channel) const {
	Channel *equiv = NULL;
	int32 index = FindChannel(channel);
	
	if (index >= 0) equiv = fChannel[index];
	
	return equiv;
};

const char *Feed::UUID(void) const {
	if (fUUID.Length() == 0) {
		uchar hash[SHA_DIGEST_LENGTH];
		char buffer[2];

		SHA1((const uchar *)fURL.String(), fURL.Length(), hash);

		for (int32 i = 0; i < SHA_DIGEST_LENGTH; i++) {
			sprintf(buffer, "%02x", hash[i]);
			
			fUUID << buffer;
		};
	};
	
	return fUUID.String();
};

//#pragma mark Operands

bool Feed::operator == (const Feed &compare) const {
	return (strcmp(UUID(), compare.UUID()) == 0);
};

bool Feed::operator != (const Feed &compare) const {
	return (strcmp(UUID(), compare.UUID()) != 0);
};

//#pragma mark Protected

void Feed::UpdateUUID(void) {
	fUUID = "";
	UUID();
};

void Feed::Name(const char *name) {
	fName = name;
};

void Feed::SetSubscribed(bool subscribe) {
	fSubscribed = subscribe;
};

//#pragma mark Private

int32 Feed::FindChannel(Channel *channel) const {
	int32 index = -1;
	BString uuid = channel->UUID();
	int32 i = 0;

	for (channel_list_t::const_iterator cIt = fChannel.begin(); cIt != fChannel.end(); cIt++) {
		Channel *current = (*cIt);
		
		if (current->UUID() == uuid) {
			index = i;
			break;
		};
		
		i++;
	};
	
	return index;
};
