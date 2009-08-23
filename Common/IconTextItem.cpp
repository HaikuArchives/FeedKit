#include "IconTextItem.h"

//#pragma mark Constants

const float kEdgeOffset = 2.0;
const rgb_color kHighlight = {140, 140, 140, 255};

//#pragma mark Constructr

IconTextItem::IconTextItem(const char *text, BBitmap *icon, bool own) 
	: fIcon(icon),
	fIconWidth(0),
	fIconHeight(0),
	fText(text),
	fOwn(own),
	fFontHeight(0),
	fFontOffset(0) {
};

IconTextItem::~IconTextItem(void) {
	if (fOwn) delete fIcon;
};

//#pragma mark BListItem Hooks

void IconTextItem::DrawItem(BView *owner, BRect frame, bool complete) {
	if (IsSelected() || complete) {
		rgb_color color;
		rgb_color origHigh;
		
		origHigh = owner->HighColor();
		
		if (IsSelected()) {
			color = kHighlight;
		} else {
			color = owner->ViewColor();
		};
		
		owner->SetHighColor(color);
		owner->FillRect(frame);
		owner->SetHighColor(origHigh);
	}
	
	if (fIcon) {
		owner->SetDrawingMode(B_OP_ALPHA);
		owner->DrawBitmap(fIcon, BPoint(frame.left + kEdgeOffset,
			frame.top + kEdgeOffset));
		owner->SetDrawingMode(B_OP_COPY);
	};
	
	if (IsSelected()) owner->SetDrawingMode(B_OP_OVER);

	owner->MovePenTo(frame.left + kEdgeOffset + fIconWidth + kEdgeOffset,
		frame.bottom - fFontOffset);
	owner->DrawString(fText.String());
};

void IconTextItem::Update(BView *owner, const BFont *font) {
	(void)owner;

	font_height fontHeight;
	font->GetHeight(&fontHeight);
	fFontHeight = fontHeight.descent + fontHeight.leading + fontHeight.ascent;
	
	if (fIcon) {
		fIconHeight = fIcon->Bounds().Height() + 1;
		fIconWidth = fIcon->Bounds().Width();

		if (fIconHeight > fFontHeight) {
			SetHeight(fIconHeight + (kEdgeOffset * 2));
			fFontOffset = ((fIconHeight - fFontHeight) / 2) + (kEdgeOffset * 2);
		} else {
			SetHeight(fFontHeight + (kEdgeOffset * 2));
			fFontOffset = kEdgeOffset;
		};
	} else {
		SetHeight(fFontHeight + (kEdgeOffset * 2));
		fFontOffset = kEdgeOffset;
	};	
};

//#pragma mark Public

const char *IconTextItem::Text(void) const {
	return fText.String();
};

const BBitmap *IconTextItem::Icon(void) const {
	return fIcon;
};
