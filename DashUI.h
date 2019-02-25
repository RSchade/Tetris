#ifndef DASHUI_H
#define DASHUI_H

#include "TetrisPiece.h"

#include <SupportDefs.h>
#include <Rect.h>
#include <View.h>
#include <StringView.h>
#include <deque>

class DashUI : public BView {
public:
			DashUI(BRect dashBox, int previews);
			~DashUI(void);
	void	SetScore(long int newScore);
	void	SetLines(int newLines);
	void	SetLevel(int newLevel);
	void	UpdatePreviews(void);
	void	SetBlockDeque(std::deque<TetrisPiece*> *newDeque);
	void	Draw(BRect rect);
	int 	previews;
	
private:
	std::deque<TetrisPiece*>	*blockDeque;
	TetrisPiece					**previewRefs;
	BStringView					*scoreView;
	BStringView					*levelView;
	BStringView					*linesView;
	BView						*blockView;
};


#endif // DASHUI_H
