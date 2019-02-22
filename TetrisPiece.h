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
				~TetrisPiece(void);
	void    	MoveTo(int x, int y);
	void    	MoveBy(int dx, int dy);
	void    	Rotate(PieceRot rot);
	BlockView	**GetBlocks(void);
	BRect		*GetPos(void);
	void		AddToView(BView &parent);
	const int	NUM_BLOCKS = 4;
private:
	PieceType	type;
	BlockView*	blocks[4];
	BPoint*		center;
};


#endif // TETRISPIECE_H
