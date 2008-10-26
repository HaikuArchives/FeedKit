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
 * Contributor(s): Wade Majors <wade@ezri.org>
 *                 Rene Gollent
 *                 Todd Lair
 *                 Andrew Bazan
 *                 Ted Stodgell <kart@hal-pc.org>
 */
 
#ifndef _RESIZEVIEW_H
#define _RESIZEVIEW_H

#include <View.h>
#include <Cursor.h>

class BMessage;
class BMessenger;

typedef enum {
	RESIZE_MODE_PROPORTIONAL,
	RESIZE_MODE_LOCK_VIEW_1,
	RESIZE_MODE_LOCK_VIEW_2
} resize_mode;

class ResizeView : public BView {
	public:
					    	ResizeView(BView *view1, BView *view2, BRect rect, const char * = "resizeView", uint32 = B_FOLLOW_LEFT | B_FOLLOW_TOP_BOTTOM, uint32 = 0);
					    	ResizeView(BMessage *archive);
	    virtual				~ResizeView (void);
	    
	    // BView Hooks
	    virtual void 		MouseDown(BPoint);
    	virtual void 		MouseMoved(BPoint, uint32 transition, const BMessage *);
    	virtual void 		MouseUp(BPoint);
    	virtual void		Draw(BRect updateRect);
    	virtual void		FrameResized(float width, float height);
    	virtual void		AttachedToWindow(void);
    	
		// BArchivable Hooks
		status_t			Archive(BMessage *archive, bool deep = true) const;
		static BArchivable	*Instantiate(BMessage *archive);
    	
    	// Public
    	void				SetVertical(bool vertical);
    	bool				IsVertical(void);
  
  		// The following resize flags should be used in conjunction with each other
  		//	RESIZE_MODE_PROPORTIONAL: View 1 should be B_FOLLOW_ALL_SIDES, View 2 should follow all
		//    sides except the one which is nearest View 1. Eg. For a horizontal arrangement View 2
		//	  would use B_FOLLOW_LEFT_RIGHT | B_FOLLOW_BOTTOM
		//	RESIZE_MODE_LOCK_VIEW_[1 | 2]: Both views should follow the sides which are not affected
		//    by the user re-dividing. Eg. In a vertical arrangement they should use
		//    B_FOLLOW_TOP_BOTTOM
  		void				SetResizeMode(resize_mode mode);
		resize_mode			ResizeMode(void);
 
	private:
		BRect				ResizeRect(void);
	
		bool				fVertical;
		BView 				*fView1;
		BView				*fView2;
		BCursor				fCursor;
		BPoint				fStartPoint;
		resize_mode			fResizeMode;
		BRect				fFrame;
		BRect				fView1Frame;
		BRect				fView2Frame;
		bool				fMouseOver;
		bool				fMousePressed;
};

#endif _RESIZEVIEW_H
