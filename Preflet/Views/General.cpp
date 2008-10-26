#include "General.h"

#include <stdio.h>

#ifdef ZETA
#include <locale/Locale.h>
#else
#define _T(str) (str)
#endif

const char *kGenLabels[] = {
	"Refresh interval", "Obey feed refresh interval", "Use feed image",
	NULL
};

const float kGenPadding = 10.0f;

GeneralView::GeneralView(BRect frame)
	: BView(frame, "GeneralView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW) {

#if B_BEOS_VERSION > B_BEOS_VERSION_5
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	SetHighColor(ui_color(B_PANEL_TEXT_COLOR));
#else
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	SetHighColor(0, 0, 0, 0);
#endif

	float widest = 0;
	for (int i = 0; kGenLabels[i] != NULL; i++) {
		float width = be_plain_font->StringWidth(_T(kGenLabels[i]));
		if (width > widest) widest = width;
	};
	widest += be_plain_font->StringWidth("  ");

	be_plain_font->GetHeight(&fFontHeight);
	float height = fFontHeight.leading + fFontHeight.ascent + fFontHeight.descent;

	BRect control(frame);
	control.OffsetTo(B_ORIGIN);
	control.bottom = control.top + height;

	fRefresh = new BTextControl(control, "Refresh", _T(kGenLabels[0]), "3", NULL);
	fRefresh->SetDivider(widest);
	fRefresh->SetAlignment(B_ALIGN_RIGHT, B_ALIGN_LEFT);
	
//	control.top = control.bottom + fFontHeight.leading + kGenPadding;
//	control.bottom = control.top + height;
	control.OffsetBy(0, height + kGenPadding);
	BRect feedRect(control);
	feedRect.left = widest;
	feedRect.right = control.right;
	fUseFeedTTL = new BCheckBox(feedRect, "UseFeed", "", NULL);
	fTTLLabel.x = widest - be_plain_font->StringWidth(_T(kGenLabels[1])) -
		be_plain_font->StringWidth(" ");
	fTTLLabel.y = feedRect.top + height;
	
//	control.top = control.bottom;
//	control.bottom = control.top + height;
	control.OffsetBy(0, height + kGenPadding);
	BRect imageRect(control);
	imageRect.right = control.right;
	imageRect.left = widest;
	fUseImage = new BCheckBox(imageRect, "UseImage", "", NULL);
	fImageLabel.x = widest - be_plain_font->StringWidth(_T(kGenLabels[2])) -
		be_plain_font->StringWidth(" ");
	fImageLabel.y = imageRect.top + height;
};

GeneralView::~GeneralView(void) {
//	fRefresh->RemoveSelf();
printf("Destructor\n");
	delete fRefresh;
	printf("Refresh\n");
	
//	fUseFeedTTL->RemoveSelf();
	delete fUseFeedTTL;
	printf("Feed\n");
	
//	fUseImage->RemoveSelf();
	delete fUseImage;
	printf("Image\n");
};

void GeneralView::Draw(BRect updateRect) {
	DrawString(_T(kGenLabels[1]), fTTLLabel);
	DrawString(_T(kGenLabels[2]), fImageLabel);
};

void GeneralView::AttachedToWindow(void) {
	AddChild(fRefresh);
	AddChild(fUseFeedTTL);
	AddChild(fUseImage);
};

void GeneralView::DetachedFromWindow(void) {
printf("Detaching\n");
	RemoveChild(fRefresh);
	printf("Refresh\n");
	RemoveChild(fUseFeedTTL);
	printf("Feed\n");
	RemoveChild(fUseImage);
	printf("Image\n");
};
			
void GeneralView::MessageReceived(BMessage *msg) {
	switch (msg->what) {
		default: {
			BView::MessageReceived(msg);
		} break;
	};
};
