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
private:
	BBitmap	*testImg;
};


#endif // BLOCKVIEW_H
