#ifndef TESTS_LIBFEEDKIT_ENCLOSURE_H
#define TESTS_LIBFEEDKIT_ENCLOSURE_H

#include <cppunit/extensions/HelperMacros.h>

class TestEnclosure : public CPPUNIT_NS::TestFixture {
	CPPUNIT_TEST_SUITE(TestEnclosure);
		CPPUNIT_TEST(Equality);
		CPPUNIT_TEST(ProgressDeletion);
	CPPUNIT_TEST_SUITE_END();

	public:
		void		setUp(void);
		void		tearDown(void);
	
	protected:
		void		Equality(void);
		void		ProgressDeletion(void);
};

#endif
