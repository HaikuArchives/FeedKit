#ifndef LIBFEEDKIT_DOWNLOADPROGRESS_H
#define LIBFEEDKIT_DOWNLOADPROGRESS_H

#include <Flattenable.h>
#include <String.h>

class FeedServer;

namespace FeedKit {

	class DownloadProgress : public BFlattenable {	
		public:
								DownloadProgress(void);
								DownloadProgress(const DownloadProgress *copy);
								~DownloadProgress(void);
	
			// BFlattenable Hooks
			status_t			Flatten(void *buffer, ssize_t numBytes) const;
			status_t			Unflatten(type_code code, const void *buffer, ssize_t numBytes);
			ssize_t				FlattenedSize(void) const;
			bool				IsFixedSize(void) const;
			type_code			TypeCode(void) const;
			bool				AllowsTypeCode(type_code code) const;
	
			// Accessor methods
			const char			*URL(void) const;
			const char			*MIME(void) const;
			const char			*Description(void) const;
			time_t				StartTime(void) const;
			time_t				EndTime(void) const;
			int32				Duration(void) const;
			float				Speed(void) const;
			int32				Size(void) const;
			int32				AmountDownloaded(void) const;
			float				PercentageComplete(void) const;
			bool				IsComplete(void) const;

		protected:
								DownloadProgress(const char *mime, time_t start,
									int32 size, int32 downloaded, time_t end = 0);
			friend class		FeedServer;

		private:
			time_t				CalculateETA(void);
		
			BString				fMIME;
			time_t				fStart;
			time_t				fEnd;
			float				fSpeed;
			int32				fSize;
			int32				fDownloaded;
			bool				fComplete;
	};
};

#endif
