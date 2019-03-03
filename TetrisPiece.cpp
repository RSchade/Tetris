#include "TetrisPiece.h"

#include <stdlib.h>

// Initializes a given tetris piece with its top left corner at (0,0)
TetrisPiece::TetrisPiece(PieceType typ)
{
	this->type = typ;
	this->size = 25;
	// TODO: abide by tetris color standards, custom colors too
	// random color for each piece
	for(int i = 0; i < NUM_BLOCKS; i++)
	{
		this->blocks[i] = new BlockView();
		this->blocks[i]->ResizeTo(this->size,this->size);	
	}
	this->color = colors[rand()%NUM_COLORS];
	SetColor(color);
	// always spawn with the flat side down
	switch(typ)
	{
		case STRAIGHT:
		{
			for(int i = 0; i < NUM_BLOCKS; i++) {
				this->blocks[i]->MoveTo(BPoint(this->size*i,0));
			}
			// TODO: this center might not be right for SRS movement
			center = new BPoint(this->size*2,0);
			break;	
		}
		case SQUARE:
		{
			this->blocks[0]->MoveTo(BPoint(0,0));
			this->blocks[1]->MoveTo(BPoint(this->size,0));
			this->blocks[2]->MoveTo(BPoint(0,this->size));
			this->blocks[3]->MoveTo(BPoint(this->size,this->size));
			center = new BPoint(this->size,this->size);
			break;
		}
		case TPOLY:
		{
			this->blocks[0]->MoveTo(BPoint(0,this->size));
			this->blocks[1]->MoveTo(BPoint(this->size,this->size));
			this->blocks[2]->MoveTo(BPoint(this->size*2,this->size));
			this->blocks[3]->MoveTo(BPoint(this->size,0));
			center = new BPoint(this->size,this->size);
			break;
		}
		case JPOLY:
		{
			this->blocks[0]->MoveTo(BPoint(0,this->size));
			this->blocks[1]->MoveTo(BPoint(this->size,this->size));
			this->blocks[2]->MoveTo(BPoint(this->size*2,this->size));
			this->blocks[3]->MoveTo(BPoint(0,0));
			center = new BPoint(this->size,this->size);
			break;
		}
		case LPOLY:
		{
			this->blocks[0]->MoveTo(BPoint(0,this->size));
			this->blocks[1]->MoveTo(BPoint(this->size,this->size));
			this->blocks[2]->MoveTo(BPoint(this->size*2,this->size));
			this->blocks[3]->MoveTo(BPoint(this->size*2,0));
			center = new BPoint(this->size,this->size);
			break;	
		}
		case SPOLY:
		{
			this->blocks[0]->MoveTo(BPoint(0,this->size));
			this->blocks[1]->MoveTo(BPoint(this->size,this->size));
			this->blocks[2]->MoveTo(BPoint(this->size,0));
			this->blocks[3]->MoveTo(BPoint(this->size*2,0));
			center = new BPoint(this->size,0);
			break;
		}
		case ZPOLY:
		{
			this->blocks[0]->MoveTo(BPoint(0,0));
			this->blocks[1]->MoveTo(BPoint(this->size,0));
			this->blocks[2]->MoveTo(BPoint(this->size,this->size));
			this->blocks[3]->MoveTo(BPoint(this->size*2,this->size));
			center = new BPoint(this->size,0);
			break;
		}
	}
}

// Copy constructor
TetrisPiece::TetrisPiece(const TetrisPiece &pc)
{
	this->size = pc.size;
	this->center = new BPoint(*pc.center);
	this->type = pc.type;
	for(int i = 0; i < this->NUM_BLOCKS; i++)
	{
		this->blocks[i] = new BlockView(*pc.blocks[i]);	
	}
}

void
TetrisPiece::SetColor(int *color)
{
	for(int i = 0; i < NUM_BLOCKS; i++)
	{
		this->blocks[i]->SetColor(color);
	}
}

// Deletes this class, does not get rid of any of the
// constituient blocks, those must be removed manually
TetrisPiece::~TetrisPiece(void)
{
	delete this->center;
}

// resize piece while keeping the center point in the same place
void
TetrisPiece::ResizeTo(int newSize)
{
	int oldSize = this->size;
	this->size = newSize;
	for(int i = 0; i < this->NUM_BLOCKS; i++)
	{
		BPoint rightBottomOld = this->blocks[i]->Frame().RightBottom();
		int blocksX = rightBottomOld.x / oldSize;
		int blocksY = rightBottomOld.y / oldSize;
		this->blocks[i]->ResizeTo(newSize,newSize);
		this->blocks[i]->MoveTo(blocksX*newSize,blocksY*newSize);
	}
}

void
TetrisPiece::AddToView(BView &parent)
{
	for(int i = 0; i < NUM_BLOCKS; i++)
	{
		parent.AddChild(this->blocks[i]);
		this->blocks[i]->Invalidate();
	}
}

void
TetrisPiece::RemoveFromParent()
{
	for(int i = 0; i < NUM_BLOCKS; i++)
	{
		this->blocks[i]->RemoveSelf();	
	}	
}


// Height/width of piece bounding boxes
int
TetrisPiece::Height()
{
	// with respect to the screen (higher means smaller number)
	int highest = -1;
	int lowest = -1;
	for(int i = 0; i < NUM_BLOCKS; i++)
	{
		BRect cur = this->blocks[i]->Frame();
		if(highest == -1 || highest > cur.top)
		{
			highest = cur.top;
		}
		if(lowest == -1 || lowest < cur.bottom)
		{
			lowest = cur.bottom;
		}
	}
	return lowest-highest;
}

int
TetrisPiece::Width()
{
	// with respect to the screen (right means smaller number)
	int rightmost = -1;
	int leftmost = -1;
	for(int i = 0; i < NUM_BLOCKS; i++)
	{
		BRect cur = this->blocks[i]->Frame();
		if(rightmost == -1 || rightmost > cur.right)
		{
			rightmost = cur.right;
		}
		if(leftmost == -1 || leftmost < cur.left)
		{
			leftmost = cur.left;
		}
	}
	return leftmost-rightmost;
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
	// if we are a square, don't rotate
	if(this->type == SQUARE)
	{
		return;
	}
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
