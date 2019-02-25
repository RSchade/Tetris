#include "BlockView.h"
#include <TranslatorFormats.h>
#include <TranslationUtils.h>

BlockView::BlockView(void)
 :	BView(BRect(0,0,25,25), "blockview", B_FOLLOW_NONE, 
 		  B_WILL_DRAW)
{
	// TODO: bitmap support, theming
	//testImg = BTranslationUtils::GetBitmap("test.bmp");
	Invalidate();
}

BlockView::BlockView(const BlockView &bview)
 : BView(BRect(0,0,25,25), "blockview", B_FOLLOW_NONE,
 		 B_WILL_DRAW)
{
	MoveTo(bview.Frame().LeftTop());
	for(int i = 0; i < 3; i++)
	{
		this->color[i] = bview.color[i];
	}	
}

BlockView::~BlockView(void) 
{
}

/*
	Sets the block's color, assumes the given pointer is to an
	array of three elements, R, G and B. Makes a copy of the given array.
*/
void BlockView::SetColor(int *color)
{
	for(int i = 0; i < 3; i++)
	{
		this->color[i] = color[i];	
	}
}

void BlockView::Draw(BRect rect) 
{
	SetDrawingMode(B_OP_ALPHA);
	SetHighColor(this->color[0], this->color[1],this->color[2]);
	FillRect(Bounds());
	// TODO: bitmap support, theming
	// DrawBitmap(testImg);
	// bevel edges
	SetHighColor(255,255,255,220);
	FillPolygon(topBev, 4);
	SetHighColor(0,0,0,100);
	FillPolygon(bottomBev, 4);
	SetHighColor(0,0,0,30);
	FillPolygon(leftBev, 4);
	FillPolygon(rightBev, 4);
}
