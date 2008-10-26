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

#include <Debug.h>

#include <Application.h>
#include <Window.h>
#include <Message.h>
 
#include "ResizeView.h"

#include <vector>

//#pragma mark Types

typedef vector<BPoint> circle_t;

//#pragma mark Constants

const float kResizeWidgetRatio = 0.1f;
const float kResizeWidgetPadding = 2.0f;
const float kResizeWidgetCircleRadius = 2.0f;
const float kResizeWidgetSpacing = 10.0f;
const int kResizeWidgetCircleCount = 2;

// horizontal resize cursor taken from OpenTracker, see www.opentracker.org for license

//const unsigned char kCursorResizeVertical[] =
//{
//	16, 1, 8, 8,
//	0, 0, 1, 0x80, 1, 0x80, 1, 0x80, 9, 0x90, 0x19, 0x98, 0x39, 0x09c, 0x79, 0x9e,
//	0x79, 0x9e, 0x39, 0x9c, 0x19, 0x98, 0x9, 0x90, 1, 0x80, 1, 0x80, 1, 0x80, 0, 0,
//	3, 0xc0, 3, 0xc0, 3, 0xc0, 0xf, 0xf0, 0x1f, 0xf8, 0x3f, 0xfa, 0x7f, 0xfe, 0xff, 0xff,
//	0xff, 0xff, 0x7f, 0xfe, 0x3f, 0xfa, 0x1f, 0xf8, 0xf, 0xf0, 3, 0xc0, 3, 0xc0, 3, 0xc0
//};
//
//const unsigned char kCursorResizeHorizontal[] = {
//	16, 1, 7, 7,
//	0, 0, 1, 0, 1, 0, 1, 0, 9, 32, 25, 48, 57, 56, 121, 60,
//	57, 56, 25, 48, 9, 32, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0,
//	3, 128, 3, 128, 3, 128, 15, 224, 31, 240, 63, 248, 127, 252, 255, 254,
//	127, 252, 63, 248, 31, 240, 15, 224, 3, 128, 3, 128, 3, 128, 0, 0
//};


//http://dev.osdrawer.net/plugins/scmsvn/viewcvs.php/src/general/SplitterView/Divider.cpp?root=libwalter&rev=76&view=markup
static const int8 kCursorResizeHorizontal[68] = { 16,1,8,10,0,0,1,128,3,192,7,224,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,7,224,3,192,1,128,0,0,0,0,1,128,3,192,7,224,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,7,224,3,192,1,128,0,0 };
static const int8 kCursorResizeVertical[68] = { 16,1,10,8,1,128,1,128,1,128,1,128,1,128,17,136,49,140,113,142,113,142,49,140,17,136,1,128,1,128,1,128,1,128,1,128,1,128,1,128,1,128,1,128,1,128,17,136,49,140,113,142,113,142,49,140,17,136,1,128,1,128,1,128,1,128,1,128 };


//#pragma mark Constructor

ResizeView::ResizeView(BView *view1, BView *view2, BRect frame, const char *title, uint32 resizeMode, uint32 flags) :
	BView(frame, title, resizeMode, flags | B_FRAME_EVENTS | B_WILL_DRAW),
	fVertical(true),
	fView1(view1),
	fView2(view2),
	fCursor(fVertical ? kCursorResizeVertical : kCursorResizeHorizontal),
	fResizeMode(RESIZE_MODE_PROPORTIONAL),
	fMouseOver(false),
	fMousePressed(false) {

	assert(fView1 != NULL);
	assert(fView2 != NULL);
	
	SetViewColor (ui_color (B_PANEL_BACKGROUND_COLOR));
	
	fView1Frame = fView1->Frame();
	fView2Frame = fView2->Frame();
};
 
ResizeView::ResizeView(BMessage *archive)
	: BView(archive),
	fCursor(fVertical ? kCursorResizeVertical : kCursorResizeHorizontal),
	fMouseOver(false),
	fMousePressed(false) {
	
	if (archive->FindBool("vertical", &fVertical) != B_OK) fVertical = false;
	if (archive->FindInt32("resize_mode", (int32 *)&fResizeMode) != B_OK) fResizeMode = RESIZE_MODE_PROPORTIONAL;

	BMessage view1;
	BMessage view2;
	
	if (archive->FindMessage("view1", &view1) == B_OK) {
		fView1 = reinterpret_cast<BView *>(instantiate_object(&view1));
	};
	if (archive->FindMessage("view2", &view2) == B_OK) {
		fView2 = reinterpret_cast<BView *>(instantiate_object(&view2));
	};
	
	ASSERT_WITH_MESSAGE(fView1 != NULL, "ResizeView::ResizeView(BMessage) - NULL View1");
	ASSERT_WITH_MESSAGE(fView2 != NULL, "ResizeView::ResizeView(BMessage) - NULL View2");

	AddChild(fView1);
	AddChild(fView2);
};
 
ResizeView::~ResizeView() {
};

//#pragma mark BView Hooks
 
void ResizeView::MouseDown(BPoint point) {
	SetMouseEventMask(B_POINTER_EVENTS, B_LOCK_WINDOW_FOCUS | B_SUSPEND_VIEW_FOCUS | B_NO_POINTER_HISTORY);

	BRect area = ResizeRect();
	
	if (area.Contains(point) == true) {	
		fStartPoint = point;
		fMousePressed = true;
		
		SetViewCursor(&fCursor);
		
		Invalidate();
	};
};
 
void ResizeView::MouseUp(BPoint point) {
	if (fMousePressed == true) {
	
		fMousePressed = false;
		be_app->SetCursor(B_HAND_CURSOR);
		
		if (fVertical) {
			BPoint difference = point - fStartPoint;
	
			fView1->ResizeBy(difference.x, 0);
	
			fView2->MoveBy(difference.x, 0);
			fView2->ResizeBy(difference.x * -1.0f, 0);
		} else {
			BPoint difference = point - fStartPoint;
	
			fView1->ResizeBy(0, difference.y);
	
			fView2->MoveBy(0, difference.y);
			fView2->ResizeBy(0, difference.y * -1.0f);
		};
		
		fFrame = Frame();
		fView1Frame = fView1->Frame();
		fView2Frame = fView2->Frame();
		
		Invalidate();
	};	
};
 
void ResizeView::MouseMoved(BPoint point, uint32 transition, const BMessage *) {
	bool original = fMouseOver;
	fMouseOver = false;

	switch (transition) {
		case B_ENTERED_VIEW:
		case B_INSIDE_VIEW: {
			BRect rect = ResizeRect();
			
//			fprintf(stderr, "ResizeView::MouseMoved(): (%.2f, %.2f) vs (%.2f, %.2f) - (%.2f, %.2f)\n", point.x, point.y, rect.left, rect.top, rect.right, rect.bottom);
			
			if (rect.Contains(point) == true) fMouseOver = true;
		} break;
	};
	
	if (fMouseOver == true) {
		be_app->SetCursor(&fCursor);
	} else {
		be_app->SetCursor(B_HAND_CURSOR);
	};
	
	if (original != fMouseOver) Invalidate();
}

void ResizeView::Draw(BRect updateRect) {
	BRect rect = ResizeRect();
	if (updateRect.Contains(rect) == false) return;

	circle_t circles;
	rgb_color view_color = ViewColor();
	
	rgb_color highlight = tint_color(view_color, B_DARKEN_1_TINT);
	rgb_color light = tint_color(view_color, B_LIGHTEN_2_TINT);
	rgb_color dark = tint_color(view_color, B_DARKEN_2_TINT);
	
	rgb_color fill = light;
	rgb_color shadow = dark;
	rgb_color outline = highlight;
			
	if (fMousePressed) {
		SetHighColor(highlight);
		FillRect(rect);
	};
	
	if (fVertical) {
		BPoint middle(rect.left + (rect.Width() / 2), rect.top + (rect.Height() / 2));

		for (int8 i = 0; i < kResizeWidgetCircleCount; i++) {
			BPoint p(middle.x, middle.y - ((i + 1) * kResizeWidgetSpacing));
			circles.push_back(p);
		};

		circles.push_back(middle);

		for (int8 i = 0; i < kResizeWidgetCircleCount; i++) {
			BPoint p(middle.x, middle.y + ((i + 1) * kResizeWidgetSpacing));
			circles.push_back(p);
		};

	} else {
		BPoint middle(rect.left + (rect.Width() / 2), rect.top + (rect.Height() / 2));

		for (int8 i = 0; i < kResizeWidgetCircleCount; i++) {
			BPoint p(middle.x - ((i + 1) * kResizeWidgetSpacing), middle.y);
			circles.push_back(p);
		};

		circles.push_back(middle);

		for (int8 i = 0; i < kResizeWidgetCircleCount; i++) {
			BPoint p(middle.x + ((i + 1) * kResizeWidgetSpacing), middle.y);
			circles.push_back(p);
		};
	};

	BPoint offset(kResizeWidgetPadding, kResizeWidgetPadding);

	for (circle_t::iterator cIt = circles.begin(); cIt != circles.end(); cIt++) {
		BPoint p = (*cIt);
	
		// Dark circle
		SetHighColor(shadow);
		FillEllipse(p, kResizeWidgetCircleRadius, kResizeWidgetCircleRadius);

		p = p - offset;
		// View
		SetHighColor(fill);
		FillEllipse(p, kResizeWidgetCircleRadius, kResizeWidgetCircleRadius);

		// Highlight
		SetHighColor(outline);
		StrokeEllipse(p, kResizeWidgetCircleRadius, kResizeWidgetCircleRadius);
	};
};

void ResizeView::FrameResized(float width, float height) {
	switch (fResizeMode) {
		case RESIZE_MODE_LOCK_VIEW_1: {
			if (fVertical) {
				fView2->ResizeBy(width - fFrame.right, 0);
			} else {
				fView2->ResizeBy(0, height - fView2Frame.bottom);
			};
		} break;
		
		case RESIZE_MODE_LOCK_VIEW_2: {
			if (fVertical) {
				fView1->ResizeBy(width - fView2Frame.right, 0);
				fView2->MoveBy(width - fFrame.Width(), 0);
			} else {
				fView1->ResizeBy(0, height - fView2Frame.bottom);
				fView2->MoveBy(0, height - fFrame.Height());
			};
		} break;
		
		default: {
			BView::FrameResized(width, height);
		} break;
	};
	
	fFrame = Frame();
	fView1Frame = fView1->Frame();
	fView2Frame = fView2->Frame();
	
	fView1->Invalidate();
	fView2->Invalidate();
	Invalidate();
};

void ResizeView::AttachedToWindow(void) {
	fFrame = Frame();
	fView1Frame = fView1->Frame();
	fView2Frame = fView2->Frame();
};

//#pragma BArchivable Hooks
status_t ResizeView::Archive(BMessage *archive, bool deep = true) const {
	status_t result = B_ERROR;
	
	archive->AddInt32("resize_mode", fResizeMode);
	archive->AddBool("vertical", fVertical);
	
	if (deep) {
		BMessage view1;
		BMessage view2;
		
		if ((fView1->Archive(&view1, deep) == B_OK) && (fView2->Archive(&view2, deep) == B_OK)) {
			result = B_OK;
			
			archive->AddMessage("view1", &view1);
			archive->AddMessage("view2", &view2);
		};
		
		BView::Archive(archive, false);
	};
	
	return result;
};

BArchivable *ResizeView::Instantiate(BMessage *archive) {
	BArchivable *instance = NULL;

	if (validate_instantiation(archive, "ResizeView") == true) {
		instance = new ResizeView(archive);
	};

	return instance;

};

//#pragma mark Public

void ResizeView::SetVertical(bool vertical) {
	fVertical = vertical;
};

bool ResizeView::IsVertical(void) {
	return fVertical;
};

void ResizeView::SetResizeMode(resize_mode mode) {
	fResizeMode = mode;
};

resize_mode ResizeView::ResizeMode(void) {
	return fResizeMode;
};

//#pragma mark Private

BRect ResizeView::ResizeRect(void) {
	BRect rect = Bounds();

	if (fVertical) {
		rect.left = fView1->Frame().right;
		rect.right = fView2->Frame().left;

		float widgetHeight = rect.Height() * kResizeWidgetRatio;
		float height = rect.Height();
		
		rect.InsetBy(kResizeWidgetPadding, kResizeWidgetPadding);
	} else {
		rect.top = fView1->Frame().bottom;
		rect.bottom = fView2->Frame().top;

		float widgetWidth = rect.Width() * 0.05f;
		float width = rect.Width();
			
		rect.InsetBy(kResizeWidgetPadding, kResizeWidgetPadding);
	};
	
	return rect;
};
