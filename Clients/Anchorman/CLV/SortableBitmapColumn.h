#ifndef SORTABLEBITMAPCOLUMN_H
#define SORTABLEBITMAPCOLUMN_H

#include "ColumnTypes.h"

class SortableBitmapField : public BBitmapField {
	public:
							SortableBitmapField(BBitmap *bitmap, int32 sort);

		int32				Sort(void);
	
	private:
		int32				fSort;
};

class SortableBitmapColumn : public BBitmapColumn {
	public:
							SortableBitmapColumn(const char *title, float width,
													 float minWidth, float maxWidth,
													 alignment align = B_ALIGN_LEFT);
							SortableBitmapColumn(BMessage *archive);

		// BColumn Hooks
		virtual int			CompareFields(BField *field1, BField *field2);					
		virtual	bool		AcceptsField(const BField* field) const;

		// BArchivable Hooks
		virtual status_t	Archive(BMessage *archive, bool deep = true) const;
		static BArchivable	*Instantiate(BMessage *archive);

};


#endif

