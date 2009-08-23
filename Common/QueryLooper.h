#ifndef QUERYLOOPER_H
#define QUERYLOOPER_H

#include <Entry.h>
#include <Looper.h>
#include <Messenger.h>
#include <String.h>
#include <Volume.h>

#include <map>
#include <vector>

class BQuery;
class BMessage;
class BHandler;

typedef struct {
	entry_ref ref;
	node_ref nref;
} result;

typedef std::map<entry_ref, result> result_t;

typedef std::vector<BVolume> volume_t;
typedef std::vector<BQuery *> query_t;

namespace QL {
	namespace Notifications {
		enum {
			InitialFetch = 'qlif',
			EntryAdded = 'qlea',
			EntryRemoved = 'qler',
		};
	};
};

class QueryLooper : public BLooper {
	public:
						QueryLooper(const char *predicate, volume_t vols,
							const char *name = NULL, BHandler *notify = NULL,
							BMessage *msg = NULL);
		virtual			~QueryLooper(void);

		// BLooper hooks
		virtual	void	MessageReceived(BMessage *msg);

		// Accessor
		int32			CountEntries(void);
		entry_ref		EntryAt(int32 index);
	
	private:
		BMessage		*fMsg;	
		BMessenger		fNotify;
			
		result_t		fResults;
		
		BString			fName;

		volume_t		fVolumes;
		query_t			fQueries;
		BString			fPredicate;
};

#endif
