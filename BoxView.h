#ifndef BOXVIEW_H
#define BOXVIEW_H


#include <SupportDefs.h>
#include <View.h>
#include <String.h>
#include <Font.h>

class BoxView : public BView {
public:
				BoxView(BRect frame, const char *name, uint32 resizingMode, 
				 		uint32 flags);
				~BoxView();
	void		Draw(BRect rect);
	void		SetTitle(BString title);
	BFont		*font;
private:
	BString		title;
};


#endif //BOXVIEW_H
