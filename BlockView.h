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
			BlockView(const BlockView &bview);
			~BlockView(void);
	void	Draw(BRect rect);
	void	SetColor(int *color);
private:
	//BBitmap				*testImg;
	int					color[3];
};


#endif // BLOCKVIEW_H
