#include "Main.h"

#include <Entry.h>
#include <Roster.h>

//#pragma mark C Hooks

extern "C" {
	void process_refs(entry_ref dir_ref, BMessage *msg, void *) {
		msg->what = B_REFS_RECEIVED;
		msg->AddRef("dir_ref", &dir_ref);
		
		be_roster->Launch("application/x-vnd.beclan.feedkit.ItemReader", msg);
	};
};

//#pragma mark Functions

int main(void) {	
	ItemReader reader;
	reader.Run();

	return 0;
};
