#ifndef MENUROW_H
#define MENUROW_H

#include "CLV/ColumnListView.h"

class BPopUpMenu;

class MenuRow : public BRow {
	public:
						MenuRow(float height = 16.0);
						~MenuRow(void);

		// BRow Hooks
		virtual void	MouseDown(BColumnListView *parent, BField *field, BPoint point, uint32 buttons);
		virtual void	MouseMoved(BColumnListView *parent, BField *field, BPoint point, uint32 buttons, int32 code);
		virtual void	MouseUp(BColumnListView *parent, BField *field);

		// Public
		void			SetMenu(BPopUpMenu *menu);
		BPopUpMenu		*Menu(void) const;
	
	private:
		BPopUpMenu		*fMenu;
};

#endif
