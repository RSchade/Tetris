#include "BoxView.h"
#include <stdio.h>

BoxView::BoxView(BRect frame, const char *name, uint32 resizingMode, 
				 uint32 flags)
	: BView(frame, name, resizingMode, flags)
{
	AdoptSystemColors();
	BFont *font = new BFont();
	this->font = font;
	SetFont(font);
}

BoxView::~BoxView()
{
}

void
BoxView::SetTitle(BString title)
{
	this->title = BString(title);
	Invalidate();
}

void
BoxView::Draw(BRect rect)
{
	SetFont(font);
	SetHighColor(0,0,0);
	BRect bound = Bounds();
	bound.top += font->Size()/1.5;
	StrokeRect(bound);
	FillRect(BRect(2,0,font->StringWidth(title)+3,font->Size()), B_SOLID_LOW);
	DrawString(this->title, BPoint(3,font->Size()));
}
