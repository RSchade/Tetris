#include "BlockView.h"
#include <TranslatorFormats.h>
#include <TranslationUtils.h>

BlockView::BlockView(void)
 :	BView(BRect(0,0,25,25), "blockview", B_FOLLOW_NONE, 
 		  B_WILL_DRAW)
{
	SetViewColor(0, 0, 100);
	//testImg = BTranslationUtils::GetBitmap("test.bmp");
	Invalidate();
}

BlockView::~BlockView(void) 
{
}

void BlockView::Draw(BRect rect) 
{
	SetDrawingMode(B_OP_ALPHA);
	SetHighColor(255,255,255);
	FillRect(Bounds());
	//DrawBitmap(testImg);
	SetHighColor(0,0,0);
	// DEBUG SHOW COORDS
	/*MovePenTo(BPoint(0,10));
	SetFontSize(6);
	char t[10];
	sprintf(t, "%d", (int)Frame().top);
	DrawString(t);
	sprintf(t, "%d", (int)Frame().bottom);
	MovePenTo(BPoint(0,20));
	DrawString(t);
	sprintf(t, "%d", (int)Frame().left);
	MovePenTo(BPoint(5,15));
	DrawString(t);
	sprintf(t, "%d", (int)Frame().right);
	MovePenTo(BPoint(15,15));
	DrawString(t);*/
	StrokeRect(Bounds());
}
