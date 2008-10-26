#ifndef LIBFEEDKIT_FEED_H
#define LIBFEEDKIT_FEED_H

#include <libfeedkit/FeedKitConstants.h>
#include <libfeedkit/ChannelSpecification.h>

#include <Flattenable.h>
#include <String.h>

#include <map>
#include <vector>

namespace FeedKit {
	class Channel;
	class FeedParser;
	
	class Feed : public BFlattenable {
		public:
							Feed(const char *url, const char *name = "");
							Feed(const Feed &rhs);
							Feed(const Feed * const rhs);
							Feed(void);
							
							~Feed(void);
								
			// BFlattenable
			status_t		Flatten(void *buffer, ssize_t numBytes) const;
			status_t		Unflatten(type_code code, const void *buffer,
								ssize_t numBytes);
			ssize_t			FlattenedSize(void) const;
			bool			IsFixedSize(void) const;
			type_code		TypeCode(void) const;
			bool			AllowsTypeCode(type_code code) const;
								
			// Public
			const char		*URL(void) const;
			const char		*Name(void) const;
			const char		*DisplayName(void) const;
			bool			Subscribed(void) const;

			// Tree traversal
			uint32			ChannelCount(void) const;
			Channel			*ChannelAt(uint32 index) const;		
			ChannelList		FindChannels(ChannelSpecification *spec, bool autodelete) const;
			Channel			*ChannelByUUID(const char *uuid) const;
			void			AddChannel(Channel *channel);

			bool			Contains(Channel *channel) const;
			Channel			*EquivalentChannel(Channel *channel) const;

			const char		*UUID(void) const;
			
			// Operands
			bool operator	== (const Feed &compare) const;
			bool operator	!= (const Feed &compare) const;
			
		protected:
			void			UpdateUUID(void);
			void			Name(const char *name);
			void			SetSubscribed(bool subscribe);
			
			friend class	FeedServer;
			friend class	FeedParser;

		private:
			int32			FindChannel(Channel *channel) const;
		
			BString			fURL;
			BString			fName;
			bool			fSubscribed;
			mutable BString	fUUID;
			
			channel_list_t	fChannel;
	};
};

#endif
