#include "TApplication.h"

int main(int argc, char *argv[]) {
	TApplication app(argc == 1);
	app.Run();
	
	return 0;
};
