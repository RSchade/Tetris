#include "TetrisPiece.h"

// Initializes a given tetris piece with its top left corner at (0,0)
// add all squares to the given BView
TetrisPiece::TetrisPiece(PieceType typ, BView &parent)
{
	this->type = typ;
	for(int i = 0; i < NUM_BLOCKS; i++)
	{
		this->blocks[i] = new BlockView();
		this->blocks[i]->ResizeTo(BLOCK_SIZE,BLOCK_SIZE);	
		parent.AddChild(this->blocks[i]);
	}
	switch(typ)
	{
		case STRAIGHT:
		{
			for(int i = 0; i < NUM_BLOCKS; i++) {
				this->blocks[i]->MoveTo(BPoint(BLOCK_SIZE*i,0));
			}
			center = new BPoint(BLOCK_SIZE*2,0);
			break;	
		}
		case SQUARE:
		{
			this->blocks[0]->MoveTo(BPoint(0,0));
			this->blocks[1]->MoveTo(BPoint(BLOCK_SIZE,0));
			this->blocks[2]->MoveTo(BPoint(0,BLOCK_SIZE));
			this->blocks[3]->MoveTo(BPoint(BLOCK_SIZE,BLOCK_SIZE));
			center = new BPoint(BLOCK_SIZE/2,BLOCK_SIZE/2);
			break;
		}
		case TPOLY:
		{
			this->blocks[0]->MoveTo(BPoint(0,0));
			this->blocks[1]->MoveTo(BPoint(BLOCK_SIZE,0));
			this->blocks[2]->MoveTo(BPoint(BLOCK_SIZE*2,0));
			this->blocks[3]->MoveTo(BPoint(BLOCK_SIZE,BLOCK_SIZE));
			center = new BPoint(BLOCK_SIZE,0);
			break;
		}
		case JPOLY:
		{
			this->blocks[0]->MoveTo(BPoint(0,0));
			this->blocks[1]->MoveTo(BPoint(BLOCK_SIZE,0));
			this->blocks[2]->MoveTo(BPoint(BLOCK_SIZE*2,0));
			this->blocks[3]->MoveTo(BPoint(BLOCK_SIZE*2,BLOCK_SIZE));
			center = new BPoint(BLOCK_SIZE,0);
			break;
		}
		case LPOLY:
		{
			this->blocks[0]->MoveTo(BPoint(0,0));
			this->blocks[1]->MoveTo(BPoint(0,BLOCK_SIZE));
			this->blocks[2]->MoveTo(BPoint(BLOCK_SIZE,0));
			this->blocks[3]->MoveTo(BPoint(BLOCK_SIZE*2,0));
			center = new BPoint(BLOCK_SIZE,0);
			break;	
		}
		case SPOLY:
		{
			this->blocks[0]->MoveTo(BPoint(0,BLOCK_SIZE));
			this->blocks[1]->MoveTo(BPoint(BLOCK_SIZE,BLOCK_SIZE));
			this->blocks[2]->MoveTo(BPoint(BLOCK_SIZE,0));
			this->blocks[3]->MoveTo(BPoint(BLOCK_SIZE*2,0));
			center = new BPoint(BLOCK_SIZE,0);
			break;
		}
		case ZPOLY:
		{
			this->blocks[0]->MoveTo(BPoint(0,0));
			this->blocks[1]->MoveTo(BPoint(BLOCK_SIZE,0));
			this->blocks[2]->MoveTo(BPoint(BLOCK_SIZE,BLOCK_SIZE));
			this->blocks[3]->MoveTo(BPoint(BLOCK_SIZE*2,BLOCK_SIZE));
			center = new BPoint(BLOCK_SIZE,0);
			break;
		}
	}
}

// Deletes this class, does not get rid of any of the
// constituient blocks, those must be removed manually
TetrisPiece::~TetrisPiece(void)
{
	delete this->center;
}

void
TetrisPiece::MoveTo(int x, int y)
{
	// figure out center's current distance away and move that amount
	BPoint dist = BPoint(x,y) - *this->center;
	MoveBy(dist.x,dist.y);
}

void
TetrisPiece::MoveBy(int dx, int dy)
{
	// move everything by an arbitrary amount, keeping all of
	// the pieces relative distances intact
	for(int i = 0; i < this->NUM_BLOCKS; i++) 
	{
		this->blocks[i]->MoveBy(dx,dy);
	}
	*this->center += BPoint(dx,dy);
}

void
TetrisPiece::Rotate(PieceRot rot)
{
	// 'about' attribute is the basis for all rotations
	BPoint about = *(this->center);
	for(int i = 0; i < this->NUM_BLOCKS; i++)
	{
		BlockView *cur = this->blocks[i];
		// Translate to the about point
		cur->MoveBy((-about).x, (-about).y);
		// Rotate using the rotation formula +90/-90
		// (-y, x) for +90, (y, -x) for -90
		BPoint coord = cur->Frame().LeftTop();
		if(rot == FORWARD) 
		{
			cur->MoveTo(BPoint(-coord.y, coord.x));
		} else if(rot == BACKWARD)
		{
			cur->MoveTo(BPoint(coord.y, -coord.x));	
		} else 
		{
			printf("INVALID ROTATION\n");	
		}
		// Translate back
		cur->MoveBy(about.x, about.y);
	}
}

BRect*
TetrisPiece::GetPos(void)
{
	// return all of the bounding boxes of all the rectangles
	BRect *boxes = new BRect[NUM_BLOCKS];
	for(int i = 0; i < NUM_BLOCKS; i++)
	{
		boxes[i] = this->blocks[i]->Frame();
	}
	return boxes;
}

BlockView**
TetrisPiece::GetBlocks(void)
{
	return this->blocks;	
}
