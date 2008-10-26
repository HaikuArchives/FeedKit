#include "Item.h"
#include "ItemSpecification.h"

using namespace FeedKit;

//pragma mark UnreadItemSpecification

bool UnreadItemSpecification::IsSatisfiedBy(Item *item) {
	return (item->Read() == false);
};

//#pragma mark CurrentItemSpecification

bool CurrentItemSpecification::IsSatisfiedBy(Item *item) {
	return item->Current();
};

//#pragma mark NewItemSpecification

bool NewItemSpecification::IsSatisfiedBy(Item *item) {
	return item->New();
};

//#pragma mark UUIDItemSpecification

UUIDItemSpecification::UUIDItemSpecification(const char *uuid)
	: fUUID(uuid) {
};
									
bool UUIDItemSpecification::IsSatisfiedBy(Item *item) {
	return (strcmp(item->UUID(), fUUID) == 0);
};

