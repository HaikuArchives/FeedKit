#ifndef LOCALENCLOSUREDOWNLOAD_H
#define LOCALENCLOSUREDOWNLOAD_H

#include <Entry.h>

namespace FeedKit {
	class Enclosure;
};

class LocalEnclosureDownload {
	public:
							LocalEnclosureDownload(void);
							LocalEnclosureDownload(entry_ref ref);
							LocalEnclosureDownload(entry_ref ref, FeedKit::Enclosure *enclosure);
							~LocalEnclosureDownload(void);
	
		// Public
		const char			*EnclosureUUID(void) const;
		bool				Cancelled(void) const;
		bool				Complete(void) const;
		int32				ExpectedSize(void) const;
		int32				CurrentSize(void) const;
	
		void				CancelDownload(void);
	private:
		entry_ref			fRef;
		
};

#endif
