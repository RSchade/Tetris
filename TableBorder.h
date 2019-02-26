#ifndef TABLEBORDER_H
#define TABLEBORDER_H


#include <SupportDefs.h>
#include <View.h>

class TableBorder: public BView {
public:
			TableBorder(BRect frame, const char *name, 
						uint32 resizingMode, uint32 flags);
			~TableBorder();
	void	Draw(BRect rect);
private:

};


#endif // _H
