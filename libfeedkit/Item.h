#ifndef LIBFEEDKIT_ITEM_H
#define LIBFEEDKIT_ITEM_H

#include <String.h>
#include <Flattenable.h>

#include <libfeedkit/FeedKitConstants.h>
#include <libfeedkit/Channel.h>
#include <libfeedkit/Content.h>
#include <libfeedkit/ContentSpecification.h>
#include <libfeedkit/Enclosure.h>
#include <libfeedkit/EnclosureSpecification.h>

namespace FeedKit {
	class Item;
	
	class Item : public BFlattenable {
		public:
								Item(void);
								Item(const Item &rhs);
								Item(const Item * const rhs);
								~Item(void);

			// BFlattenable Hooks
			status_t			Flatten(void *buffer, ssize_t numBytes) const;
			status_t			Unflatten(type_code code, const void *buffer, ssize_t numBytes);
			ssize_t				FlattenedSize(void) const;
			bool				IsFixedSize(void) const;
			type_code			TypeCode(void) const; 
			bool				AllowsTypeCode(type_code code) const;
	
			// Public
			void				ParentChannel(Channel *channel);
			Channel				*ParentChannel(void) const;
			
			bool				IsGUIDPermaLink(void) const;
			const char			*GUID(void) const;
			const char			*Author(void) const;
			const char 			*Title(void) const;
			const char			*Link(void) const;
			const char			*Category(void) const;
			const char			*Comments(void) const;
			time_t				Date(void) const;
				
			const char			*SourceURL(void) const;
			const char			*SourceTitle(void) const;
	
			uint32				EnclosureCount(void) const;
			Enclosure			*EnclosureAt(uint32 index) const;
			EnclosureList		FindEnclosures(EnclosureSpecification *spec, bool autodelete) const;

			Enclosure			*EnclosureByUUID(const char *uuid) const;
			void				AddEnclosure(Enclosure *enclosure);
	
			uint32				ContentCount(void) const;
			Content				*ContentAt(uint32 index) const;
			ContentList			FindContent(ContentSpecification *spec, bool autodelete) const;
			void				AddContent(Content *content);
	
			bool				Read(void) const;
			bool				Current(void) const;
			bool				New(void) const;
	
			void				IsGUIDPermaLink(bool permalink);
			void				GUID(const char *guid);
			void				Author(const char *author);
			void 				Title(const char *title);
			void				Link(const char *link);
			void				Category(const char *category);
			void				Comments(const char *comments);
			void				Date(time_t date);
				
			void				SourceURL(const char *url);
			void				SourceTitle(const char *title);
	
			const char			*UUID(void) const;
			const char			*LocalUUID(void) const;
			
			// Operands
			bool operator		== (const Item &compare) const;
			bool operator		!= (const Item &compare) const;

		protected:
			void				SetRead(bool read);
			void				SetCurrent(bool current);
			void				SetNew(bool isNew);
			void				UpdateUUID(void);

			friend class		FeedServer;
	
		private:
			Channel				*fChannel;
			BString				fChannelUUID;
			
			mutable BString		fUUID;
			mutable BString		fLocalUUID;
			
			bool				fPermaLinkGUID;
			BString				fGUID;
			BString				fAuthor;
			BString				fTitle;
			BString				fLink;
			BString				fCategory;
			BString 			fComments;
			time_t				fDate;
			
			BString				fSourceURL;
			BString 			fSourceTitle;
		
			// Misc
			bool				fRead;
			bool				fCurrent;
			bool				fNew;

			enclosure_list_t	fEnclosure;
			content_list_t		fContent;
	};
};

#endif
