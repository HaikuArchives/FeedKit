#ifndef GENERAL_H
#define GENERAL_H

#include <CheckBox.h>
#include <TextControl.h>
#include <View.h>

class GeneralView : public BView {
	public:
						GeneralView(BRect frame);
						~GeneralView(void);
					
				void	Draw(BRect updateRect);
				void	AttachedToWindow(void);
				void	DetachedFromWindow(void);
		
				void	MessageReceived(BMessage *msg);
	private:
		BTextControl	*fRefresh;
			BCheckBox	*fUseFeedTTL;
			BPoint		fTTLLabel;
			BCheckBox	*fUseImage;
			BPoint		fImageLabel;

			font_height	fFontHeight;
};

#endif
