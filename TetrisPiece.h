#ifndef TETRISPIECE_H
#define TETRISPIECE_H

#include "BlockView.h"

#include <SupportDefs.h>
#include <View.h>
#include <Polygon.h>

enum PieceType
{
	STRAIGHT = 0,
	SQUARE = 1,
	TPOLY = 2,
	JPOLY = 3,
	LPOLY = 4,
	SPOLY = 5,
	ZPOLY = 6
};

enum PieceRot
{
	FORWARD = 0,
	BACKWARD = 1
};

const int BLOCK_SIZE = 25;

class TetrisPiece {
public:
				TetrisPiece(PieceType typ);
				TetrisPiece(const TetrisPiece &pc);
				~TetrisPiece(void);
	void    	MoveTo(int x, int y);
	void    	MoveBy(int dx, int dy);
	void    	Rotate(PieceRot rot);
	void		ResizeTo(int newSize);
	BlockView	**GetBlocks(void);
	BRect		*GetPos(void);
	void		AddToView(BView &parent);
	void		RemoveFromParent(void);
	int			Height();
	int			Width();
	const int	NUM_BLOCKS = 4;
	int			size; // size of each block
	PieceType	type;
private:
	BlockView			*blocks[4];
	BPoint				*center;
	static const int	NUM_COLORS = 7;
	// TODO: make colors definable in resource/theme file
	int					colors[NUM_COLORS][3] = {{0, 240, 240},
							{0, 0, 240}, {240, 160, 0}, {240, 240, 0},
							{0, 240, 0}, {160, 0, 240}, {240, 0, 0}};
};


#endif // TETRISPIECE_H
