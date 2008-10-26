#ifndef ROWTAG_H
#define ROWTAG_H

#include "MenuRow.h"

template <class T>
class RowTag : public MenuRow {
	public:
							RowTag(float height, T tag)
								: MenuRow(height),
								fTag(tag) {
							};
							
							RowTag(T tag)
								: MenuRow(),
								fTag(tag) {
							};
	
		T					Tag(void) {
								return fTag;
							};
							
		void				SetTag(T tag) {
								fTag = tag;
							};
	private:
		T					fTag;
};

#endif
