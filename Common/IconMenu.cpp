/* 
 * The contents of this file are subject to the Mozilla Public 
 * License Version 1.1 (the "License"); you may not use this file 
 * except in compliance with the License. You may obtain a copy of 
 * the License at http://www.mozilla.org/MPL/ 
 * 
 * Software distributed under the License is distributed on an "AS 
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or 
 * implied. See the License for the specific language governing 
 * rights and limitations under the License. 
 * 
 * The Original Code is Vision.
 * 
 * The Initial Developer of the Original Code is The Vision Team.
 * Portions created by The Vision Team are
 * Copyright (C) 1999, 2000, 2001 The Vision Team.  All Rights
 * Reserved.
 * 
 * Contributor(s): Rene Gollent
 *                 Alan Ellis <alan@cgsoftware.org>
 */
 
//------------------------------------------------------------------------------
// IconMenu.cpp
//------------------------------------------------------------------------------
// A menu item implementation that displays an icon as its label.
//
// IconMenu implementation Copyright (C) 1998 Tyler Riti <fizzboy@mail.utexas.edu>
// Based on code Copyright (C) 1997 Jens Kilian
// This code is free to use in any way so long as the credits above remain intact.
// This code carries no warranties or guarantees of any kind. Use at your own risk.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// I N C L U D E S
//------------------------------------------------------------------------------
#include <be/storage/AppFileInfo.h>
#include <be/interface/Bitmap.h>
#include <be/app/Application.h>
#include <be/app/Roster.h>

#include "IconMenu.h"

//------------------------------------------------------------------------------
// I M P L E M E N T A T I O N
//------------------------------------------------------------------------------

TIconMenu::TIconMenu(BBitmap* icon, BMenu* menu) :
        BMenuItem(menu),
        bounds(),
        iconLabel(NULL)
{
    if (icon) {
        bounds = icon->Bounds();
        iconLabel = new BBitmap(bounds, B_COLOR_8_BIT);
        iconLabel->SetBits(icon->Bits(), icon->BitsLength(), 0, B_COLOR_8_BIT);
    }
}

TIconMenu::TIconMenu(BMenu* menu) :
        BMenuItem(menu),
        bounds(0.0, 0.0, 15.0, 15.0),
        iconLabel(NULL)
{
    app_info info;
    if (be_app->GetAppInfo(&info) == B_NO_ERROR) {
        BFile appFile(&(info.ref), O_RDONLY);
        BAppFileInfo appFileInfo(&appFile);

        iconLabel = new BBitmap(bounds, B_COLOR_8_BIT);

        if (appFileInfo.GetIcon(iconLabel, B_MINI_ICON) != B_NO_ERROR) {
            delete iconLabel;
            iconLabel = NULL;
        }
    }
}

TIconMenu::TIconMenu(BMessage *archive)
	: BMenuItem(archive),
		iconLabel(NULL) {
	
	BMessage iconMsg;
	if (archive->FindMessage("icon", &iconMsg) == B_OK) {
		iconLabel = dynamic_cast<BBitmap *>(instantiate_object(&iconMsg));
	};
	archive->FindRect("bounds", &bounds);
};

TIconMenu::~TIconMenu()
{
    delete iconLabel;
    iconLabel = NULL;
}

void TIconMenu::GetContentSize(float* width, float* height)
{
    if (iconLabel) {
        *width = bounds.Width();
        *height = bounds.Height();
    }
    else
        BMenuItem::GetContentSize(width, height);
}

void TIconMenu::DrawContent()
{
    if (iconLabel) {
        Menu()->SetDrawingMode(B_OP_OVER);

        float width, height;

        Menu()->GetPreferredSize(&width, &height);

        BRect destBounds = bounds;
        destBounds.OffsetBy(8.0f, ((height - bounds.Height()) * 0.5f) - 1);

        // Scaling the icon is left as an exercise for the reader :)
        Menu()->DrawBitmap(iconLabel, bounds, destBounds);
    }
    else
        BMenuItem::DrawContent();
}

//-------------------------------------------------------------- IconMenu.cpp --

//#pragma mark BArchivable Hooks

status_t TIconMenu::Archive(BMessage *archive, bool deep) const {
	status_t result = BMenuItem::Archive(archive, deep);
	if (result == B_OK) {
		archive->ReplaceString("class", "TIconMenu");
	
		if (iconLabel) {
			BMessage icon;
			iconLabel->Archive(&icon, true);
			archive->AddMessage("icon", &icon);
		};
		
		archive->AddRect("bounds", bounds);
	};
	
	return result;
};

BArchivable *TIconMenu::Instantiate(BMessage *archive) {
	BArchivable *instance = NULL;
	if (validate_instantiation(archive, "TIconMenu") == true) {
		instance = new TIconMenu(archive);
	};
	
	return instance;
};

