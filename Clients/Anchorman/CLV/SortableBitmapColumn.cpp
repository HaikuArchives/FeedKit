#include "SortableBitmapColumn.h"

//#pragma mark SortableBitmapField - Constructor

SortableBitmapField::SortableBitmapField(BBitmap *bitmap, int32 sort)
	: BBitmapField(bitmap),
	fSort(sort) {
};

//#pragma mark SortableBitmapField - Public
	
int32 SortableBitmapField::Sort(void) {
	return fSort;
};

//#pragma mark SortableBitmapColumn - Constructor

SortableBitmapColumn::SortableBitmapColumn(const char *title, float width, float minWidth,
	float maxWidth, alignment align = B_ALIGN_LEFT)

	: BBitmapColumn(title, width, minWidth, maxWidth, align) {
};
						
SortableBitmapColumn::SortableBitmapColumn(BMessage *archive)
	: BBitmapColumn(archive) {
};

//#pragma mark SortableBitmapColumn - BColumn Hooks

int SortableBitmapColumn::CompareFields(BField *field1, BField *field2) {
	SortableBitmapField *f1 = (SortableBitmapField *)field1;
	SortableBitmapField *f2 = (SortableBitmapField *)field2;

	return f1->Sort() - f2->Sort();
};
						
bool SortableBitmapColumn::AcceptsField(const BField* field) const {
	return static_cast<bool>(dynamic_cast<const SortableBitmapField *>(field));
};

//#pragma mark SortableBitmapColumn - BArchivable Hooks

status_t SortableBitmapColumn::Archive(BMessage *archive, bool deep = true) const {
	status_t result = BBitmapColumn::Archive(archive, deep);
	archive->ReplaceString("class", "SortableBitmapColumn");
	
	return result;
};

BArchivable *SortableBitmapColumn::Instantiate(BMessage *archive) {
	BArchivable *instance = NULL;
	if (validate_instantiation(archive, "SortableBitmapColumn") == true) {
		instance = new SortableBitmapColumn(archive);
	};
	
	return instance;
};
