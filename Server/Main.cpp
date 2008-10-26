#include "FeedServer.h"

#if 0
#include <Message.h>
#include <String.h>
#include <map>

#include <Flattenable.h>

class FlatTest : public BFlattenable {
	public:
	int32 value;

	// BFlattenable Hooks
	status_t			Flatten(void *buffer, ssize_t numBytes) const {
		BMessage msg;
		msg.AddInt32("value", value);

printf("Value is %i\n", value);

		return msg.Flatten((char *)buffer, numBytes);
	};
	
	status_t			Unflatten(type_code code, const void *buffer, ssize_t numBytes) {
		BMessage msg;
		status_t result = msg.Unflatten((char *)buffer);
		
		if (result == B_OK) {
			if (msg.FindInt32("value", &value) != B_OK) result = B_ERROR;
		};
		
		return result;
	};
	
	ssize_t				FlattenedSize(void) const {
		BMessage msg;
		msg.AddInt32("value", value);
		
		return msg.FlattenedSize();
	};

	bool				IsFixedSize(void) const {
		return true;
	};
	type_code			TypeCode(void) const {
		return 'flat';
	};
	
	bool				AllowsTypeCode(type_code code) const {
		return code == 'flat';
	};

	
};

class DelayMessage : public BMessage {
	public:
		status_t AddDelayedFlat(const char *name, BFlattenable *flat) {
			fFlat[name] = flat;
			
			return B_OK;
		};

		status_t AddFlat(const char *name, BFlattenable *flat) {
			return AddDelayedFlat(name, flat);
		};

		operator BMessage *() {
			BMessage *msg = this;
			
			map<BString, BFlattenable *>::iterator iIt;
			for (iIt = fFlat.begin(); iIt != fFlat.end(); iIt++) {
				msg->AddFlat(iIt->first.String(), (iIt->second));
			};
			
			return msg;
		}
		
	private:
		map<BString, BFlattenable *> fFlat;
	
};

#include "ObjectUpdateFilter.h"
#endif

int main(void) {
	FeedServer server;
	server.Run();
	
	return 0;
	
#if 0
//	ObjectUpdateFilter filter(&server);
//	FlatTest *test = new FlatTest();
//	test->value = 0;
//
//	DelayedFlattenedObject progressUpdateCommand("flat", test);
//	BMessage progress;
//	progress.AddFlat("delayedupdate", &progressUpdateCommand);
//
//	test->value = 0xffffffff;
//	
//	progress.PrintToStream();
//	
//	printf("Flattening...\n");
//	
//	bool flat = filter.FlattenDelayedObjects(&progress);
//	printf("Has flat: %s\n", flat == true ? "yup" : "nope");


//	FlatTest *test = new FlatTest();
//	BMessage msg;
//	msg.AddMessage
	
//	server.Run();

//	DelayMessage msg;
//	FlatTest *test = new FlatTest();
//	test->value = 0;
	
//	msg.AddDelayedFlat("blah", test);
//	msg.AddFlat("blah", test);	
//	test->value = 100;
	
//	msg.PrintToStream();
//
//	BMessage *bmsg = msg;
//	
//	bmsg->PrintToStream();
//
//	FlatTest two;
//	bmsg->FindFlat("blah", &two);
//	
//	printf("Value: %i\n", two.value);
#endif
};
