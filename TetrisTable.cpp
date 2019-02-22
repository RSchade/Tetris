#include "TetrisTable.h"
#include "BlockView.h"

#include <OS.h>
#include <Window.h>
#include <vector>
#include <File.h>
#include <Directory.h>

int32
tetris_clock(void *params)
{
	TetrisTable *table = (TetrisTable *)params;
	while(1) {
		table->Tick();
		// TODO: speed up as the game goes on
		snooze(1000000);
	}
	return 0;
}

TetrisTable::TetrisTable(void)
 :	BView(BRect(0,0,WIDTH,HEIGHT), "tetristable", B_FOLLOW_NONE, B_WILL_DRAW)
{	
	// TODO: make matrix resizable
	for(int i = 0; i < rowSize; i++)
	{
		for(int j = 0; j < colSize; j++)
		{
			bottomMatrix[i][j] = NULL;
		}
	}
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
	SetHighColor(0,0,0);
	StrokeRect(Bounds());
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
	// TODO: make mouse inputs do nothing during pause
}

// Creates a new piece for the tetris table
void
TetrisTable::NewPiece()
{
	// if there are less than 10 blocks queued up, add more
	const int toQueue = 10;
	if(this->nextBlocks.size() < toQueue/2)
	{
		BFile random(new BDirectory("/dev"), "urandom", B_READ_ONLY);
		if(random.InitCheck() != B_OK)
		{
			printf("error with getting random numbers\n");
			// TODO: quit
		}
		// gather 10 random bytes
		unsigned char *bytes = new unsigned char[toQueue];
		random.Read(bytes, toQueue);
		for(int i = 0; i < toQueue; i++)
		{
			// 7 types of pieces
			int randPiece = (int)(bytes[i] % 7);
			// this->nextBlocks.push(new TetrisPiece((PieceType)randPiece));
			this->nextBlocks.push(new TetrisPiece((PieceType)randPiece));
		}	
	}
	// TODO: add 'next' on screen
	// TODO: game levels
	// pop the next piece off the queue and put it in the right place
	this->pc = this->nextBlocks.front();
	this->pc->AddToView(*this);
	this->pc->MoveTo(150,25);
	this->nextBlocks.pop();
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
			MoveActive(0,BLOCK_SIZE);
		} else {
			// Create a new piece at the top
			NewPiece();
		}
		// If we can, free some rows
		FreeRows();
		Window()->UnlockLooper();
	}
}

// Find contiguous rows of pieces and frees them from the board
// Moves the rest of the game board down to fill in the gaps
void
TetrisTable::FreeRows()
{
	// TODO: Issues with freeing rows, sometimes whole rows aren't freed
	// and other times rows are freed when they aren't supposed to be
	// pieces drop too much sometimes after they free a row
	
	// keep track of which rows we are deleting
	std::vector<int> delRows(0);
	bool full = true;
	// search through every row, cataloging the full ones
	for(int i = 0; i < rowSize; i++)
	{
		for(int j = 0; j < colSize; j++)
		{
			if(bottomMatrix[i][j] == NULL)
			{
				// move to the next row
				full = false;
				break;
			}	
		}
		if(full == true)
		{
			delRows.push_back(i);
			for(int j = 0; j < colSize; j++)
			{
				bottomMatrix[i][j]->RemoveSelf();
				delete bottomMatrix[i][j];
				bottomMatrix[i][j] = NULL;
			}
		}
		full = true;
	}

	// TODO: scoring system
	// LEVEL:  1		2			3			4
	// POINTS: 40*(n+1) 100*(n+1)	300*(n+1)	1200*(n+1)
	// where n is the current level
	
	// move all the blocks down to fill the gap
	// for every delRows move everything above it down by 1
	// then offset 1 from every other delRows
	int delRowsOffset = 0;
	for(int row = 0; row < delRows.size(); row++)
	{
		for(int i = delRows[row]+delRowsOffset; i > 0; i--)
		{
			for(int j = 0; j < colSize; j++)
			{
				bottomMatrix[i][j] = bottomMatrix[i-1][j];
				bottomMatrix[i-1][j] = NULL;
				if(bottomMatrix[i][j] != NULL)
				{
					bottomMatrix[i][j]->MoveBy(0,BLOCK_SIZE);	
				}
			}
		}
		
		delRowsOffset++;	
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
		// check if this piece is moving beyond the screen
		if(BeyondScreen(bx))
		{
			bx = 0;
		}
		this->pc->MoveBy(bx,by);
	}
}

bool
TetrisTable::BeyondScreen(int bx)
{
	// check if this piece is moving beyond the screen
	BlockView **blocks = this->pc->GetBlocks();
	for(int i = 0; i < this->pc->NUM_BLOCKS; i++)
	{
		BRect curFrame = blocks[i]->Frame();
		if(curFrame.left+bx >= WIDTH || curFrame.right+bx < BLOCK_SIZE)
		{
			return true;
			break;	
		}
	}
	return false;
}

// Collides with the bottom/floor if the current block touches it
// makes the current block become apart of the floor
TetrisTable::CollisionType
TetrisTable::CheckCollision()
{
	// TODO: Still issue with some collisions
	// TODO: do neighbor collision model instead
	
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
			if(curRect.bottom > Bounds().Height()-BLOCK_SIZE 
			   || curCollide == STICK)
			{
				// TODO: before something sticks, allow it to move a little bit
				// but still have it collide
				
				// make this become apart of the blocks at the
				// bottom of the screen
				BlockView **newBlocks = this->pc->GetBlocks();
				for(int j = 0; j < this->pc->NUM_BLOCKS; j++)
				{
					BRect curFrame = newBlocks[j]->Frame();
					int row = (int)(curFrame.top/BLOCK_SIZE);
					int col = (int)(curFrame.left/BLOCK_SIZE);
					// add to matrix
					this->bottomMatrix[row][col] = newBlocks[j];
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
	for(int i = 0; i < rowSize; i++)
	{
		for(int j = 0; j < colSize; j++) 
		{
			if(!bottomMatrix[i][j])
			{
				continue;
			}
			BRect curBottom = bottomMatrix[i][j]->Frame();
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
			if(col > NONE || BeyondScreen(0))
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
				MoveActive(0,BLOCK_SIZE);
				break;
			case B_LEFT_ARROW:
				MoveActive(-BLOCK_SIZE,0);
				break;
			case B_RIGHT_ARROW:
				MoveActive(BLOCK_SIZE,0);
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
