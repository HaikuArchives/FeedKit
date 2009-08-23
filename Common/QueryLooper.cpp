#include "QueryLooper.h"

#include <MessageFilter.h>
#include <NodeMonitor.h>
#include <Query.h>

#include "IMKitUtilities.h"
#include <stdio.h>

using namespace QL;

//#pragma mark Constants

const int32 msgInitialFetch = 'ql01';

//#pragma mark Constructor

QueryLooper::QueryLooper(const char *predicate, volume_t vols,
	const char *name, BHandler *notify, BMessage *msg)
	: BLooper(name),
		fMsg(NULL) {
	
	fName = name;
	if (notify) fNotify = BMessenger(notify);
	fPredicate = predicate;
	fVolumes = vols;

	if (msg) fMsg = new BMessage(*msg);
	
	Run();
	
	BMessenger(this).SendMessage(msgInitialFetch);
};

QueryLooper::~QueryLooper(void) {
	if (fMsg) delete fMsg;
	
	int32 queries = fQueries.size();
	for (int32 i = 0; i < queries; i++) delete fQueries[i];
};

//#pragma mark BLooper Hooks

void QueryLooper::MessageReceived(BMessage *msg) {
	switch (msg->what) {
		case B_QUERY_UPDATE: {
			int32 opcode = 0;
			entry_ref affected;
			if (msg->FindInt32("opcode", &opcode) != B_OK) return;
					
			switch (opcode) {
				case B_ENTRY_CREATED: {
					result r;
					const char *name;
				
					msg->FindInt32("device", &r.ref.device); 
					msg->FindInt64("directory", &r.ref.directory); 
					msg->FindString("name", &name); 
					r.ref.set_name(name);

					msg->FindInt32("device", &r.nref.device);
					msg->FindInt64("node", &r.nref.node);
										
					fResults[r.ref] = r;
					affected = r.ref;
				} break;
				
				case B_ENTRY_REMOVED: {
					node_ref nref;
					result_t::iterator rIt;

					msg->FindInt32("device", &nref.device);
					msg->FindInt64("node", &nref.node);

					for (rIt = fResults.begin(); rIt != fResults.end(); rIt++) {
						result r = rIt->second;
						
						if (nref == r.nref) {
							fResults.erase(r.ref);
							affected = r.ref;
							break;
						};
					};
				} break;
			};
		
			if ((fNotify.IsValid()) && (fMsg != NULL)) {
				BMessage notify(*fMsg);
				notify.AddString("qlName", fName);
				notify.AddRef("affected_ref", &affected);
				
				if (opcode == B_ENTRY_CREATED) {
					notify.AddInt32("query_what", Notifications::EntryAdded);
				} else {
					notify.AddInt32("query_what", Notifications::EntryRemoved);
				};

#if B_BEOS_VERSION > B_BEOS_VERSION_5				
				fNotify.SendMessage(notify);
#else
				fNotify.SendMessage(&notify);
#endif
			};
		} break;
		
		case msgInitialFetch: {
			volume_t::iterator vIt;
			for (vIt = fVolumes.begin(); vIt != fVolumes.end(); vIt++) {
				BVolume vol = (*vIt);
				BQuery *query = new BQuery();
		
				query->SetPredicate(fPredicate.String());
				query->SetTarget(this);
				query->SetVolume(&vol);
		
				query->Fetch();
		
				entry_ref ref;
				while (query->GetNextRef(&ref) == B_OK) {
					BNode node(&ref);
					result r;
					r.ref = ref;
					node.GetNodeRef(&r.nref);

					fResults[ref] = r;
				};
				
				fQueries.push_back(query);
			};
			
			if ((fNotify.IsValid()) && (fMsg != NULL)) {
				BMessage notify(*fMsg);
				notify.AddString("qlName", fName);
				notify.AddInt32("query_what", Notifications::InitialFetch);

#if B_BEOS_VERSION > B_BEOS_VERSION_5				
				fNotify.SendMessage(notify);
#else
				fNotify.SendMessage(&notify);
#endif
			};

		} break;
		
		default: {
			BLooper::MessageReceived(msg);
		};
	};
};

//#pragma mark Public

int32 QueryLooper::CountEntries(void) {
	return fResults.size();
};

entry_ref QueryLooper::EntryAt(int32 index) {
	entry_ref ref;
	if ((index >= 0) && (index < CountEntries())) {
		result_t::iterator rIt = fResults.begin();
		int32 i = 0;
		
		for (i = 0; i < index && rIt != fResults.end(); i++, rIt++) {
		};
		
		if (i == index) ref = rIt->first;
	}
	
	return ref;
};
