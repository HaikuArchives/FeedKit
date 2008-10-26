#include "MenuRow.h"

#include <PopUpMenu.h>

//#pragma mark Constructor

MenuRow::MenuRow(float height = 16.0)
	: BRow(height),
	fMenu(NULL) {
};

MenuRow::~MenuRow(void) {
	delete fMenu;
}					

//#pragma mark BRow Hooks
void MenuRow::MouseDown(BColumnListView *parent, BField */*field*/, BPoint point, uint32 buttons) {
	if ((buttons == B_SECONDARY_MOUSE_BUTTON) && (fMenu != NULL)) {
		BPoint p2 = parent->ScrollView()->ConvertToScreen(point);
		p2.x -= 5.0;
		p2.y -= 5.0;

		fMenu->Go(p2, true, false, true);
	};
};

void MenuRow::MouseMoved(BColumnListView */*parent*/, BField */*field*/, BPoint /*point*/, uint32 /*buttons*/, int32 /*code*/) {
};

void MenuRow::MouseUp(BColumnListView */*parent*/, BField */*field*/) {
};

//#pragma mark Public
void MenuRow::SetMenu(BPopUpMenu *menu) {
	delete fMenu;
	fMenu = menu;
};

BPopUpMenu *MenuRow::Menu(void) const {
	return fMenu;
};
