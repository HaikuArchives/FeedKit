#ifndef LIBFEEDKIT_CONTENTSPECIFICATION_H
#define LIBFEEDKIT_CONTENTSPECIFICATION_H

#include <vector>

#include <libfeedkit/Specification.h>

namespace FeedKit {
	class Feed;
	
	typedef std::vector<Content *> ContentList;
	typedef Specification<Content *> ContentSpecification;
	
	class AllContentSpecification : public ContentSpecification {
		public:
			virtual bool		IsSatisfiedBy(Content */*content*/) {
									return true;
								};
	};
	
	class MIMEContentSpecification : public ContentSpecification {
		public:
								MIMEContentSpecification(const char *type)
									: ContentSpecification(),
									fMIME(type) {
								};
		
			virtual bool		IsSatisfiedBy(Content *content) {
									return (fMIME == content->MIMEType());
								};
		
		private:
			BString				fMIME;
	};
	
	class MIMESuperTypeContentSpecification : public ContentSpecification {
		public:
								MIMESuperTypeContentSpecification(const char *super)
									: ContentSpecification(),
									fSuperType(super) {
								};
		
			virtual bool		IsSatisfiedBy(Content *content) {
									return (fSuperType.ICompare(content->MIMEType(), fSuperType.Length()) == 0);
								};
		
		private:
			BString				fSuperType;
	};
};

#endif
