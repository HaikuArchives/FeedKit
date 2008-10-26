#include <Application.h>
#include <Deskbar.h>
#include <Path.h>
#include <Roster.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Constants.h"
#include "FeedIcon.h"

int main(int argc, char *argv[]) {
	BDeskbar deskbar;
	app_info info;
	
	new BApplication(APP_SIG);

	// Remove the old instance
	if (deskbar.HasItem(VIEW_NAME) == true) deskbar.RemoveItem(VIEW_NAME);

	if (argc == 1) {
		if (be_app->GetAppInfo(&info) == B_OK) {
			status_t result = deskbar.AddItem(&info.ref, NULL);
			printf("Adding deskbar icon (%s): %s\n", info.ref.name, strerror(result));
		} else {
			printf("Unable to obtain app info\n");
		};
	};
	
	return B_OK;
};
