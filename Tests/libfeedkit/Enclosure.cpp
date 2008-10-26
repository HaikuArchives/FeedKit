#include "Enclosure.h"

#include <libfeedkit/Enclosure.h>
#include <libfeedkit/DownloadProgress.h>

CPPUNIT_TEST_SUITE_REGISTRATION(TestEnclosure);

using namespace FeedKit;

class FeedServer {
	public:
		static void SetProgress(Enclosure *enclosure, DownloadProgress *progress) {
						enclosure->SetProgress(progress);
		};
};

//#pragma mark Test Fixture Hooks

void TestEnclosure::setUp(void) {
};
					
void TestEnclosure::tearDown(void) {
};
	
//#pragma mark Tests
	
void TestEnclosure::Equality(void) {
	Enclosure *a = new Enclosure("http://www.CompanyA.com.au", "text/plain");
	Enclosure *b = new Enclosure("http://www.CompanyB.com.au", "text/plain");
	Enclosure *UpperA = new Enclosure("HTTP://WWW.COMPANYA.COM.AU", "TEXT/PLAIN");
	
	CPPUNIT_ASSERT_MESSAGE("Comparison unexpectedly passed", a != b);
	CPPUNIT_ASSERT_MESSAGE("Case sensitive comparison should fail", a != UpperA);
};

void TestEnclosure::ProgressDeletion(void) {
	Enclosure *enclosure = new Enclosure();
	DownloadProgress *progress = new DownloadProgress();
	
	FeedServer::SetProgress(enclosure, progress);
//	enclosure->SetProgress(progress);
	
	delete progress;
	progress = NULL;
	
	progress = new DownloadProgress();
	FeedServer::SetProgress(enclosure, progress);
//	enclosure->SetProgress(progress);
};