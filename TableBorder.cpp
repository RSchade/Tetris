#include "TableBorder.h"
	
TableBorder::TableBorder(BRect frame, const char *name, 
	uint32 resizingMode, uint32 flags) : 
	BView(frame, name, resizingMode, flags)
{
}

TableBorder::~TableBorder()
{
}

void
TableBorder::Draw(BRect rect)
{
	SetPenSize(10);
	SetHighColor(0,0,0);
	StrokeRect(Bounds());
}
