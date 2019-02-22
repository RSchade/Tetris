#ifndef BLOCKVIEW_H
#define BLOCKVIEW_H


#include <SupportDefs.h>
#include <Message.h>
#include <Bitmap.h>
#include <View.h>
#include <stdio.h>
#include <Point.h>

class BlockView : public BView {
public:
			BlockView(void);
			~BlockView(void);
	void	Draw(BRect rect);
	void	SetColor(int *color);
private:
	BBitmap				*testImg;
	int					color[3];
	static const int 	inset = 4; // amount to put the bevels in by
	const BPoint 		topBev[4] = {BPoint(0,0), BPoint(inset,inset), 
									 BPoint(26-inset,inset), BPoint(26,0)};
	const BPoint 		bottomBev[4] = {BPoint(0,26), BPoint(inset,26-inset), 
									 	BPoint(26-inset,26-inset), BPoint(26,26)};
	const BPoint 		leftBev[4] = {BPoint(0,0), BPoint(inset,inset), 
									  BPoint(inset,26-inset), BPoint(0,26)};
	const BPoint 		rightBev[4] = {BPoint(26,0), BPoint(26-inset,inset), 
									   BPoint(26-inset,26-inset), BPoint(26,26)};
};


#endif // BLOCKVIEW_H
