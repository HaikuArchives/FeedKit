#ifndef LIBFEEDKIT_CHANNEL_H
#define LIBFEEDKIT_CHANNEL_H

#include <libfeedkit/FeedKitConstants.h>
#include <libfeedkit/ItemSpecification.h>
#include <libfeedkit/EnclosureSpecification.h>

#include <Flattenable.h>
#include <String.h>

#include <map>

namespace FeedKit {
	class Feed;
	class Item;
	class Enclosure;
	
	typedef std::map<BString, Item *> item_cache_t;
	
	class Channel : public BFlattenable {
		public:
								Channel(void);
								Channel(const Channel &rhs);
								Channel(const Channel * const rhs, bool deep = true);
								~Channel(void);

			// BFlattenable Hooks
			status_t			Flatten(void *buffer, ssize_t numBytes) const;
			status_t			Unflatten(type_code code, const void *buffer, ssize_t numBytes);
			ssize_t				FlattenedSize(void) const;
			bool				IsFixedSize(void) const;
			type_code			TypeCode(void) const;
			bool				AllowsTypeCode(type_code code) const;
	
			// Compulsory elements
			const char			*Title(void) const;
			const char			*Link(void) const;
			const char			*Description(void) const;
	
			// Optional elements
			const char			*Language(void) const;
			const char			*Copyright(void) const;
			const char			*Editor(void) const;
			const char			*Webmaster(void) const;
			time_t				PublishedDate(void) const;
			time_t				BuildDate(void) const;
			const char			*Category(void) const;
			const char			*Generator(void) const;
			const char			*Docs(void) const;
			long				TTL(void) const;
			const char			*PICSRating(void) const;
	
			Enclosure			*Image(void) const;
	
			// Iteration
			uint32				ItemCount(void) const;
			Item				*ItemAt(uint32 index) const;
			Item				*ItemByUUID(const char *uuid) const;
			
			uint32				EnclosureCount(void) const;
			Enclosure			*EnclosureAt(uint32 index) const;

			// Searching			
			ItemList			FindItems(ItemSpecification *spec, bool autodelete) const;
			EnclosureList		FindEnclosures(EnclosureSpecification *spec, bool autodelete) const;
	
			// Public
			void				ParentFeed(Feed *parent);
			Feed				*ParentFeed(void) const;
							
			bool				Contains(Item *item) const;

			// Setting
			void				Title(const char *title);
			void				Link(const char *link);
			void				Description(const char *description);
			void				Language(const char *language);
			void				Copyright(const char *copyright);
			void				Editor(const char *editor);
			void				Webmaster(const char *webmaster);
			void				PublishedDate(time_t published);
			void				BuildDate(time_t build);
			void				Category(const char *category);
			void				Generator(const char *generator);
			void				Docs(const char *docs);
			void				TTL(long ttl);
			void				PICSRating(const char *pics);
			void				Image(Enclosure *image);
		
			void				AddItem(Item *item);
			void				AddEnclosure(Enclosure *enclosure);
			
			const char			*UUID(void) const;
			const char			*LocalUUID(void) const;
	
			// Operands
			bool operator		== (const Channel &compare) const;
			bool operator		!= (const Channel &compare) const;
		protected:
			void				UpdateUUID(void);
						
			friend class		FeedServer;
	
		private:
			Feed				*fFeed;
			BString				fFeedUUID;
			
			mutable BString		fUUID;
			mutable BString 	fLocalUUID;
			BString				fTitle;
			BString				fLink;
			BString				fDescription;
					
			BString				fLanguage;
			BString				fCopyright;
			BString				fEditor;
			BString				fWebmaster;
			time_t				fPublishedDate;
			time_t				fLastBuildDate;
			BString				fCategory;
			BString				fGenerator;
			BString				fDocs;
//			BString				fCloud;
			long				fTTL;
			BString				fPICSRating;
//			BString				fTextInput;
			
			BString				fImageUUID;
			Enclosure			*fImage;
			item_list_t			fItem;
			
			enclosure_list_t	fEnclosure;

//			textInput, skipHours, skipDays
	};
};

#endif
