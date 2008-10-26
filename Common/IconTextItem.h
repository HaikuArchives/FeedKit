#ifndef LIBFEEDKIT_ICONTEXTITEM_H
#define LIBFEEDKIT_ICONTEXTITEM_H

#include <Bitmap.h>
#include <ListItem.h>
#include <String.h>
#include <View.h>

class IconTextItem : public BListItem {
	public:
	
						IconTextItem(const char *text, BBitmap *icon = NULL,
							bool own = true);
						~IconTextItem(void);
				
		// BListItem Hooks		
		virtual void	DrawItem(BView *owner, BRect frame, bool complete);
		virtual void	Update(BView *owner, const BFont *font);
		
		// Public
		const char 		*Text(void) const;
		const BBitmap	*Icon(void) const;	

	private:
		BBitmap			*fIcon;
		float			fIconWidth;
		float			fIconHeight;

		BString			fText;
		bool			fOwn;

		float			fFontHeight;
		float			fFontOffset;
};

template <class T>
class IconTextItemTag : public IconTextItem {
	public:
						IconTextItemTag(const char *text, BBitmap *icon = NULL,
							bool own = true)
							: IconTextItem(text, icon, own) {
						};

		T				Data(void) {
							return fData;
						};
		void			SetData(T data) {
							fData = data;
						};

	private:
		T				fData;
};


#endif
