#include "IMKitUtilities.h"

#include <stdio.h>

const int32 kSmallIcon = 16;
const int32 kLargeIcon = 32;

#ifdef B_ZETA_VERSION
#include <sys_apps/Tracker/Icons.h>
#endif

// Loads the icon. Callers responsibility to free BBitmap

BBitmap *ReadNodeIcon(const char *name, int32 size, bool followSymlink) {
	BBitmap *ret = NULL;

#if B_BEOS_VERSION > B_BEOS_VERSION_5
 #if B_BEOS_VERSION > B_BEOS_VERSION_DANO
  #ifndef B_ZETA_VERSION
    // Zeta RC2 or early code.
	BEntry entry(name, followSymlink);
    entry_ref ref;
	BPath path;
	
	entry.GetRef(&ref);
	BNode node(BPath(&ref).Path());

	ret = GetTrackerIcon(node, size, NULL);
  #else
   // Zeta RC3 or later.
	BEntry entry(name, followSymlink);
    entry_ref ref;
	BPath path;
	
	entry.GetRef(&ref);

   ret = new BBitmap(GetTrackerIcon(entry, size));
  #endif
 #else
  // Dano code.
   if (size == kSmallIcon) {
		ret = GetBitmapFromAttribute(name, BEOS_SMALL_ICON_ATTRIBUTE, 'MICN', followSymlink);
	} else {
		ret = GetBitmapFromAttribute(name, BEOS_LARGE_ICON_ATTRIBUTE, 'ICON', followSymlink);
	};
 #endif
#else
 // R5 code.
 if (size == kSmallIcon) {
		ret = GetBitmapFromAttribute(name, BEOS_SMALL_ICON_ATTRIBUTE, 'MICN', followSymlink);
	} else {
		ret = GetBitmapFromAttribute(name, BEOS_LARGE_ICON_ATTRIBUTE, 'ICON', followSymlink);
	};
#endif
	
	return ret;
};

// Loads 'attribute' of 'type' from file 'name'. Returns a BBitmap (Callers 
//  responsibility to delete it) on success, NULL on failure. 

BBitmap *GetBitmapFromAttribute(const char *name, const char *attribute, 
	type_code type, bool followSymlink) {

	BEntry entry(name, followSymlink);
	entry_ref ref;
	BPath path;
	
	entry.GetRef(&ref);

	BBitmap 	*bitmap = NULL;
	size_t 		len = 0;

	if ((name == NULL) || (attribute == NULL)) return NULL;

	BNode node(BPath(&ref).Path());
	
	if (node.InitCheck() != B_OK) {
		return NULL;
	};
	
	attr_info info;
		
	if (node.GetAttrInfo(attribute, &info) != B_OK) {
		node.Unset();
		return NULL;
	};
		
	char *data = (char *)calloc((size_t)info.size, sizeof(char));
	len = (size_t)info.size;
		
	if (node.ReadAttr(attribute, type, 0, data, len) != (size_t)len) {
		node.Unset();
		free(data);
	
		return NULL;
	};
	
//	Icon is a square, so it's right / bottom co-ords are the root of the bitmap length
//	Offset is 0
	BRect bound = BRect(0, 0, 0, 0);
	bound.right = (float)sqrt(len) - 1;
	bound.bottom = bound.right;
	
	bitmap = new BBitmap(bound, B_COLOR_8_BIT);
	bitmap->SetBits(data, len, 0, B_COLOR_8_BIT);

//	make sure it's ok
	if(bitmap->InitCheck() != B_OK) {
		free(data);
		delete bitmap;
		return NULL;
	};

	return bitmap;
}

// Reads attribute from node. Returns contents (to be free()'d by user) or NULL on
// fail

char *ReadAttribute(BNode node, const char *attribute, int32 *length) {
	attr_info info;
	char *value = NULL;

	if (node.GetAttrInfo(attribute, &info) == B_OK) {
		value = (char *)calloc((size_t)info.size, sizeof(char));
		if (node.ReadAttr(attribute, info.type, 0, (void *)value, (size_t)info.size) !=
			(size_t)info.size) {
			
			free(value);
			value = NULL;
		};
		if (length) *length = (size_t)info.size;
	};

	return value;
};

status_t WriteAttribute(BNode node, const char *attribute, const char *value,
	size_t length, type_code type) {

	status_t ret = B_ERROR;
	if ((ret = node.InitCheck()) == B_OK) {
		ret = node.WriteAttr(attribute, type, 0, value, length);
	};
	
	return ret;
};
