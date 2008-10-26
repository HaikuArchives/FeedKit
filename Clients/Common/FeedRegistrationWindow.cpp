#include "FeedRegistrationWindow.h"

#include <Box.h>
#include <Button.h>
#include <CheckBox.h>
#include <Messenger.h>
#include <TextControl.h>
#include <View.h>

//#pragma mark Constants

const float kPadding = 10.0f;
const int32 kMsgURLChanged = 'sw01';
const int32 kMsgSubscribe = 'sw02';
const int32 kMsgCancel = 'sw03';

//#pragma mark Constructor

FeedRegistrationWindow::FeedRegistrationWindow(BMessenger *target, BMessage *cancel, BMessage *subscribe)
	: BWindow(BRect(100, 100, 425, 220), "Register a Feed", B_TITLED_WINDOW,
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_ASYNCHRONOUS_CONTROLS),
	
	fTarget(target),
	fMsgCancel(cancel),
	fMsgSubscribe(subscribe),
	fView(NULL),
	fBox(NULL),
	fName(NULL),
	fURL(NULL),
	fAutoUpdate(NULL),
	fSubscribe(NULL),
	fCancel(NULL) {
	
	BRect rect = Bounds();
	
	fView = new BView(rect, "ContainerView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
#if B_BEOS_VERSION > B_BEOS_VERSION_5
	fView->SetViewUIColor(B_UI_PANEL_BACKGROUND_COLOR);
	fView->SetLowUIColor(B_UI_PANEL_BACKGROUND_COLOR);
	fView->SetHighUIColor(B_UI_PANEL_TEXT_COLOR);
#else
	fView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	fView->SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	fView->SetHighColor(0, 0, 0, 255);
#endif	
	AddChild(fView);
	
	const char *labels[] = { "URL", "Name", "Auto-update", NULL };
	float labelWidth = 0;
	float width = 0;
	float height = 0;
	
	for (int32 i = 0; labels[i] != NULL; i++) {
		labelWidth = max_c(labelWidth, be_plain_font->StringWidth(labels[i]));
	};
	labelWidth += kPadding;
	
	rect.InsetBy(kPadding, kPadding);
	
	fBox = new BBox(rect, "", B_FOLLOW_ALL_SIDES);
	fView->AddChild(fBox);
	
	rect = fBox->Bounds();
	rect.InsetBy(kPadding, kPadding);
	
	fName = new BTextControl(rect, "FeedName", "Feed Name", "", NULL, B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP,
		B_WILL_DRAW | B_NAVIGABLE);
	fName->GetPreferredSize(&width, &height);
	fName->ResizeTo(rect.Width(), height);
	fName->SetDivider(labelWidth);
	fName->SetAlignment(B_ALIGN_RIGHT, B_ALIGN_LEFT);
	fBox->AddChild(fName);
	
	rect.top = fName->Frame().bottom + kPadding;
	fURL = new BTextControl(rect, "FeedURL", "URL", "", new BMessage(kMsgURLChanged),
		B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE);
	fURL->GetPreferredSize(&width, &height);
	fURL->ResizeTo(rect.Width(), height);
	fURL->SetDivider(labelWidth);
	fURL->SetAlignment(B_ALIGN_RIGHT, B_ALIGN_LEFT);
	fBox->AddChild(fURL);
	
	rect.top = fURL->Frame().bottom + kPadding;	
	fSubscribe = new BButton(rect, "Subscribe", "Subscribe", new BMessage(kMsgSubscribe),
		B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM, B_WILL_DRAW | B_NAVIGABLE);
	fSubscribe->GetPreferredSize(&width, &height);
	fSubscribe->ResizeTo(width, height);
	fSubscribe->MoveTo(rect.right - width, rect.top);
	fSubscribe->SetEnabled(false);
	fSubscribe->MakeDefault(true);
	fBox->AddChild(fSubscribe);
	
	fCancel = new BButton(rect, "Cancel", "Cancel", new BMessage(kMsgCancel),
		B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM, B_WILL_DRAW | B_NAVIGABLE);
	fCancel->GetPreferredSize(&width, &height);
	fCancel->ResizeTo(width, height);
	fCancel->MoveTo(fSubscribe->Frame().left - kPadding - width, rect.top);
	fBox->AddChild(fCancel);
	
	fName->MakeFocus(true);
};

FeedRegistrationWindow::~FeedRegistrationWindow(void) {
	delete fTarget;
	delete fMsgCancel;
	delete fMsgSubscribe;

	if (fName) fName->RemoveSelf();
	delete fName;
	
	if (fURL) fURL->RemoveSelf();
	delete fURL;
	
	if (fAutoUpdate) fAutoUpdate->RemoveSelf();
	delete fAutoUpdate;
	
	if (fSubscribe) fSubscribe->RemoveSelf();
	delete fSubscribe;
	
	if (fCancel) fCancel->RemoveSelf();
	delete fCancel;

	if (fBox) fBox->RemoveSelf();
	delete fBox;

	if (fView) fView->RemoveSelf();
	delete fView;

};

//#pragma mark BWindow Hooks

bool FeedRegistrationWindow::QuitRequested(void) {
	//return BWindow::QuitRequested();
	return true;
};

void FeedRegistrationWindow::MessageReceived(BMessage *msg) {
	switch (msg->what) {
		case kMsgURLChanged: {
			fSubscribe->SetEnabled(strlen(fURL->Text()) > 0);
		} break;
	
		case kMsgSubscribe: {
			BMessage subscribe(*fMsgSubscribe);
			subscribe.AddString("url", fURL->Text());
			subscribe.AddString("name", fName->Text());
			fTarget->SendMessage(&subscribe);
			
			Quit();
		} break;
		
		case kMsgCancel: {
			BMessage cancel(*fMsgCancel);
			fTarget->SendMessage(&cancel);
			
			Quit();
		} break;
	
		default: {
			BWindow::MessageReceived(msg);
		};
	};
};

//#pragma mark Public

//#pragma mark Private
