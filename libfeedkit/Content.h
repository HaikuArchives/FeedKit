#ifndef LIBFEEDKIT_CONTENT_H
#define LIBFEEDKIT_CONTENT_H

#include <String.h>
#include <Flattenable.h>

#include <libfeedkit/FeedKitConstants.h>

namespace FeedKit {
	class Item;

	class Content : public BFlattenable {
		public:
								Content(void);
								Content(const Content &rhs);
								Content(const Content * const rhs);
								~Content(void);

			// BFlattenable Hooks
			status_t			Flatten(void *buffer, ssize_t numBytes) const;
			status_t			Unflatten(type_code code, const void *buffer, ssize_t numBytes);
			ssize_t				FlattenedSize(void) const;
			bool				IsFixedSize(void) const;
			type_code			TypeCode(void) const; 
			bool				AllowsTypeCode(type_code code) const;
	
			// Public
			void				ParentItem(Item *item);
			Item				*ParentItem(void) const;
			
			const char			*MIMEType(void) const;
			const char			*Summary(void) const;
			const char			*Text(void) const;
			
			void				MIMEType(const char *type);
			void				Summary(const char *summary);
			void				Text(const char *text);
			
			const char 			*UUID(void) const;
			const char			*LocalUUID(void) const;

			// Operands
			bool operator		== (const Content &compare) const;
			bool operator		!= (const Content &compare) const;

		protected:
			void				UpdateUUID(void);

			friend class		FeedServer;	
		private:
			Item				*fItem;
			BString				fItemUUID;
			
			mutable BString		fUUID;
			mutable BString		fLocalUUID;
			
			BString				fMIMEType;
			BString				fSummary;
			BString				fText;
	};
};

#endif
