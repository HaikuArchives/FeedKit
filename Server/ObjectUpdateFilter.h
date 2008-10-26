#ifndef OBJECTUPDATEFILTER_H
#define OBJECTUPDATEFILTER_H

#include <Flattenable.h>
#include <MessageFilter.h>
#include <String.h>

class ObjectSource;

enum ObjectUpdateCommandType {
	UpdateFeed,
	UpdateChannel,
	UpdateItem,
	UpdateEnclosure,
	UpdateNone
};

class DelayedFlattenedObject : public BFlattenable {
	public:
								DelayedFlattenedObject(void);
								DelayedFlattenedObject(const char *name, BFlattenable *object);
	
		// Public
		const char				*Name(void) const;
		BFlattenable			*Object(void) const;
	
		// BFlattenable
		status_t				Flatten(void *buffer, ssize_t numBytes) const;
		status_t				Unflatten(type_code code, const void *buffer, ssize_t numBytes);
		ssize_t					FlattenedSize(void) const;
		bool					IsFixedSize(void) const;
		type_code				TypeCode(void) const;
		bool					AllowsTypeCode(type_code code) const;

	private:
		BString					fName;
		BFlattenable			*fObject;
};

class ObjectUpdateFilter : public BMessageFilter {
	public:
							ObjectUpdateFilter(ObjectSource *source);
							~ObjectUpdateFilter(void);
					
		// BMessageFilter Hooks		
		filter_result		Filter(BMessage *msg, BHandler **target);

		// Public
		bool				FlattenDelayedObjects(BMessage *msg);
		
	private:
		ObjectSource		*fSource;
};

#endif
