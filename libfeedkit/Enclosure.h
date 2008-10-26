#ifndef LIBFEEDKIT_ENCLOSURE_H
#define LIBFEEDKIT_ENCLOSURE_H

#include <libfeedkit/FeedKitConstants.h>

#include <Entry.h>
#include <Flattenable.h>
#include <String.h>

class FeedServer;

namespace FeedKit {
	class DownloadProgress;

	enum EnclosureState {
		None,
		Queued,
		Downloading,
		Cancelled,
		Error,
		Completed
	};

	class Enclosure : public BFlattenable {
		public:
								Enclosure(void);
								Enclosure(const Enclosure &rhs);
								Enclosure(const Enclosure * const rhs);
								Enclosure(const char *url, const char *mime = NULL,
									const char *description = NULL,	int32 size = -1);
								~Enclosure(void);
	
			// BFlattenable Hooks
			status_t			Flatten(void *buffer, ssize_t numBytes) const;
			status_t			Unflatten(type_code code, const void *buffer, ssize_t numBytes);
			ssize_t				FlattenedSize(void) const;
			bool				IsFixedSize(void) const;
			type_code			TypeCode(void) const;
			bool				AllowsTypeCode(type_code code) const;
			
			// Public
			const char			*URL(void) const;
			const char			*MIME(void) const;
			const char			*Description(void) const;
			int32				Size(void) const;

			// Download Information
			EnclosureState		State(void) const;
			const DownloadProgress
								*Progress(void) const;
			entry_ref			LocalRef(void) const;

			const char 			*UUID(void) const;
			const char			*LocalUUID(void) const;
			
			// Operands
			bool operator		== (const Enclosure &compare) const;
			bool operator		!= (const Enclosure &compare) const;

		protected:
			void				SetLocalRef(entry_ref ref);
			void				SetState(EnclosureState state);
			void				SetProgress(DownloadProgress *progress);

			friend class		FeedServer;

		private:
			BString				fURL;
			BString				fMIME;
			BString				fDescription;
			int32				fSize;
			EnclosureState		fState;
			DownloadProgress	*fProgress;
		
			mutable BString		fLocalUUID;
			entry_ref			fLocalRef;
	};
};

#endif
