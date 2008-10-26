#ifndef LIBFEEDKIT_ERRORDETAILS_H
#define LIBFEEDKIT_ERRORDETAILS_H

#include <String.h>
#include <Flattenable.h>

namespace FeedKit {

	class ErrorDetails  : public BFlattenable {
		public:
								ErrorDetails(void);
								ErrorDetails(int32 code, const char *message);
								~ErrorDetails(void);
								
			// BFlattenable Hooks
			status_t			Flatten(void *buffer, ssize_t numBytes) const;
			status_t			Unflatten(type_code code, const void *buffer, ssize_t numBytes);
			ssize_t				FlattenedSize(void) const;
			bool				IsFixedSize(void) const;
			type_code			TypeCode(void) const; 
			bool				AllowsTypeCode(type_code code) const;
		
			int32				Code(void);
			const char			*Message(void);
						
		private:
			int32				fCode;
			BString				fMessage;
	};
};

#endif
