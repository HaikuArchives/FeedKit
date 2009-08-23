#ifndef LIBFEEDKIT_ITEMSPECIFICATION_H
#define LIBFEEDKIT_ITEMSPECIFICATION_H

#include <vector>

#include <libfeedkit/Specification.h>

namespace FeedKit {
	class Item;

	typedef std::vector<Item *> ItemList;
	typedef Specification<Item *> ItemSpecification;

	class UnreadItemSpecification : public ItemSpecification {
		public:
			virtual bool 			IsSatisfiedBy(Item *item);
	};
	
	class CurrentItemSpecification : public ItemSpecification {
		public:
			virtual bool			IsSatisfiedBy(Item *item);
	};
	
	class NewItemSpecification : public ItemSpecification {
		public:
			virtual bool			IsSatisfiedBy(Item *item);
	};

	class UUIDItemSpecification : public ItemSpecification {
		public:
									UUIDItemSpecification(const char *uuid);
									
			virtual bool			IsSatisfiedBy(Item *item);
		private:
			const char				*fUUID;
	};
};

#endif
