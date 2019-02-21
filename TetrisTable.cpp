#include "TetrisTable.h"
#include "BlockView.h"

#include <OS.h>
#include <Window.h>

int32
tetris_clock(void *params)
{
	TetrisTable *table = (TetrisTable *)params;
	while(1) {
		table->Tick();
		snooze(1000000);
	}
	return 0;
}

TetrisTable::TetrisTable(void)
 :	BView(BRect(0,0,400,400), "tetristable", B_FOLLOW_ALL, B_WILL_DRAW)
{
	Bounds().PrintToStream();
	Frame().PrintToStream();
	
	time_thread = spawn_thread(tetris_clock, "tetrisclock", 
							   B_REAL_TIME_PRIORITY, this);
	if(time_thread == B_NO_MORE_THREADS) {
		printf("Error making clock thread\n");
		// TODO: exit	
	}
	
	NewPiece();
}

TetrisTable::~TetrisTable(void) 
{
}

void 
TetrisTable::Draw(BRect rect) 
{
}

void
TetrisTable::Start()
{
	printf("thread: %d\n", time_thread);
	resume_thread(time_thread);
}

void
TetrisTable::Pause()
{
	suspend_thread(time_thread);
}

// Creates a new piece for the tetris table
void
TetrisTable::NewPiece()
{
	// TODO: randomization and 'next'
	this->pc = new TetrisPiece(ZPOLY, *this);
	this->pc->MoveTo(150,0);
}

// Called by a worker thread every time the main clock advances
// main loop of the game
void
TetrisTable::Tick()
{
	if(Window() != NULL && Window()->LockLooper()) 
	{
		if(this->pc != NULL)
		{
			// Lower the active piece
			MoveActive(0,25);
		} else {
			// Create a new piece at the top
			NewPiece();
		}
		// If we can, free a row
		
		Window()->UnlockLooper();
	}
}

// moves the active piece by a number of blocks in the x and y
// directions, stops if it hits anything and removes the piece
// from play
void
TetrisTable::MoveActive(int bx, int by)
{
	CollisionType col = CheckCollision();
	if(this->pc != NULL) 
	{
		if(col == COLLIDEL)
		{
			// collided, at least don't allow the piece to move 
			// horizontally into this area
			if(bx < 0)
			{
				bx = 0;
			}
		} else if(col == COLLIDER)
		{
			if(bx > 0)
			{
				bx = 0;
			}	
		} else if(col == COLLIDE)
		{
			// collided on both sides, don't allow it to move 
			// horizontally at all
			bx = 0;
		}
		this->pc->MoveBy(bx,by);
	}
}

// Collides with the bottom/floor if the current block touches it
// makes the current block become apart of the floor
TetrisTable::CollisionType
TetrisTable::CheckCollision()
{
	CollisionType ret = NONE;
	if(this->pc != NULL)
	{
		BRect *rects = this->pc->GetPos();
		for(int i = 0; i < this->pc->NUM_BLOCKS; i++)
		{
			BRect curRect = rects[i];
			CollisionType curCollide = CollidesBottomBlocks(curRect);
			// make the block stop if it hits the blocks at the bottom
			// or if it hits the bottom of the game board
			if(curRect.bottom > Bounds().Height()-25 
			   || curCollide == STICK)
			{
				// make this become apart of the blocks at the
				// bottom of the screen
				BlockView **newBlocks = this->pc->GetBlocks();
				for(int j = 0; j < this->pc->NUM_BLOCKS; j++)
				{
					this->bottomBlocks.push_back(newBlocks[j]);
				}
				delete this->pc;
				this->pc = NULL;
				return STICK;
			}
			if(ret == COLLIDE || (ret == COLLIDEL && curCollide == COLLIDER)
			                  || (ret == COLLIDER && curCollide == COLLIDEL))
			{
				return COLLIDE;
			} else if(ret == NONE)
			{
				ret = curCollide;
			}
		}
	}
	return ret;
}

TetrisTable::CollisionType 
TetrisTable::CollidesBottomBlocks(BRect rect)
{
	// return the most dominant term
	CollisionType ret = NONE;
	for(int i = 0; i < this->bottomBlocks.size(); i++)
	{
		BRect curBottom = bottomBlocks[i]->Frame();
		bool alignBot = (int)curBottom.bottom == (int)rect.bottom;
		bool touchL = (int)curBottom.right == (int)rect.left;
		bool touchR = (int)curBottom.left == (int)rect.right;
		if((int)curBottom.top == (int)rect.bottom
		   &&  (int)curBottom.right == (int)rect.right
		   &&  (int)curBottom.left == (int)rect.left)
		{
			return STICK;
		} else if(touchL && touchR && alignBot)
		{
			ret = COLLIDE;
		} else if(touchL && alignBot)
		{
			if(ret == COLLIDER)
			{
				ret = COLLIDE;
			} else if(ret == NONE)
			{
				ret = COLLIDEL;
			}
		} else if(touchR && alignBot)
		{
			if(ret == COLLIDEL)
			{
				ret = COLLIDE;
			} else if(ret == NONE)
			{
				ret = COLLIDER;
			}
		}
	}
	return ret;
}

void
TetrisTable::RotateActive(PieceRot rot)
{
	if(this->pc != NULL)
	{
		this->pc->Rotate(rot);
		BlockView **blocks = this->pc->GetBlocks();
		for(int i = 0; i < this->pc->NUM_BLOCKS; i++)
		{
			CollisionType col = CollidesBottomBlocks(blocks[i]->Frame());
			if(col > NONE)
			{
				if(rot == FORWARD)
				{
					this->pc->Rotate(BACKWARD);
				} else
				{
					this->pc->Rotate(FORWARD);
				}
				break;
			}
		}
	}	
}

void
TetrisTable::KeyDown(const char *bytes, int32 numBytes)
{
	if(numBytes == 1)
	{
		switch(bytes[0])
		{
			case B_DOWN_ARROW:
				MoveActive(0,25);
				break;
			case B_LEFT_ARROW:
				MoveActive(-25,0);
				break;
			case B_RIGHT_ARROW:
				MoveActive(25,0);
				break;
			case 'z':
				RotateActive(BACKWARD);
				break;
			case 'x':
				RotateActive(FORWARD);
				break;
		}	
	}
}
