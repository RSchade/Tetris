#include "TetrisTable.h"
#include "BlockView.h"
#include "7Bag.h"
#include "TGMRand.h"

#include <OS.h>
#include <Window.h>
#include <vector>
#include <File.h>
#include <Directory.h>
#include <Message.h>

#include <StopWatch.h>

int32
tetris_clock(void *params)
{
	TetrisTable *table = (TetrisTable *)params;
	while(1) {
		table->Tick();
		// TODO: speed up as the game goes on
		snooze(1000000 - table->level*5000);
	}
	return 0;
}

// width and height are in blocks
TetrisTable::TetrisTable(DashUI *ui, int rowSize, int colSize)
 :	BView(BRect(0,0,colSize*BLOCK_SIZE,rowSize*BLOCK_SIZE), 
    "tetristable", B_FOLLOW_NONE, B_WILL_DRAW)
{	
	this->dashUi = ui;
	this->dashUi->SetBlockDeque(&this->nextBlocks);
	this->randomizer = new TGMRand(3, 6);
	// initialize board size constants and array
	this->rowSize = rowSize;
	this->colSize = colSize;
	this->bottomMatrix = new BlockView**[rowSize];
	for(int i = 0; i < this->rowSize; i++)
	{
		this->bottomMatrix[i] = new BlockView*[colSize];
		for(int j = 0; j < this->colSize; j++)
		{
			this->bottomMatrix[i][j] = NULL;
		}
	}
	this->level = 0;
	this->score = 0;
	this->lines = 0;
	this->shiftTime = -1;
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
	if(this->nextBlocks.size() <= this->dashUi->previews)
	{
		// queue up twice what the dashboard needs to display the previews
		for(int i = 0; i < 20; i++)
		{
			// if it's the first time do the start condition logic
			TetrisPiece *newPc = this->randomizer->GetNextPiece();
			while(nextBlocks.size() == 0 && 
			     (newPc->type == SPOLY || newPc->type == ZPOLY || newPc->type == SQUARE))
			{
				// TODO: customize these in a different layer?
				// game never starts with S, Z or O (for TGM and some other implementations)
				BlockView **blocks = newPc->GetBlocks();
				for(int j = 0; j < newPc->NUM_BLOCKS; j++)
				{
					delete blocks[j];
				}
				delete newPc;
				newPc = this->randomizer->GetNextPiece();
				printf("HAD TO PICK NEW START PIECE\n");
			}
			this->nextBlocks.push_front(newPc);
		}
	}
	
	// pop the next piece off the queue and put it in the right place
	this->shiftTime = -1;
	this->pc = this->nextBlocks.front();
	this->pc->AddToView(*this);
	BPoint spawn = GetSpawnLoc(this->pc);
	this->pc->MoveTo(spawn.x,spawn.y);
	this->nextBlocks.pop_front();	
	// update 'next' piece on screen
	this->dashUi->UpdatePreviews();
}

BPoint
TetrisTable::GetSpawnLoc(TetrisPiece *piece)
{
	// TODO: customize spawn location
	// Tetris spec is to start I/O in the middle, rest spawn left middle
	// spawn horizontal, flat side first
	// row 21 or 22
	int spawnY = 22*BLOCK_SIZE;
	int spawnX = 0;
	switch(piece->type)
	{
		case STRAIGHT:
		case SQUARE:
			spawnX = (colSize/2)*BLOCK_SIZE;
			break;
		default:
			spawnX = (colSize-1)/2 * BLOCK_SIZE;
			break;
	}
	return BPoint(spawnX,spawnY);
}

// Called by a worker thread every time the main clock advances
// main loop of the game
void
TetrisTable::Tick()
{
	if(Window() != NULL && Window()->LockLooper()) 
	{
		//BStopWatch *w = new BStopWatch("ticktime");
		if(this->pc != NULL)
		{
			// Lower the active piece
			MoveActive(0,BLOCK_SIZE);
		} 
		//w->Lap();
		
		// need this, state can change after MoveActive
		if(this->pc == NULL) {
			// Check for rows to free at the bottom
			FreeRows();
			//w->Lap();
			// Check for lose state
			if(TopOut(this->nextBlocks.front()))
			{
				printf("GAME LOST\n");
				Pause();
			} else
			{
				// Create a new piece at the top
				NewPiece();
				//w->Lap();
			}
		}
		//w->Lap();
		//delete w;
		
		Window()->UnlockLooper();
	}
}

bool
TetrisTable::TopOut(TetrisPiece *next)
{
	// TODO: customize conditions
	// Tops out if the spawn of a new piece overlaps at least 1 block
	// or if there is a piece above the visible playfield (20 from bottom)
	BPoint spawn = GetSpawnLoc(next);
	next->MoveTo(spawn.x,spawn.y);
	BRect *blocks = next->GetPos();
	for(int i = 0; i < next->NUM_BLOCKS; i++)
	{
		BPoint curBlock = blocks[i].LeftTop();
		int curRow = (int)(curBlock.y/BLOCK_SIZE);
		int curCol = (int)(curBlock.x/BLOCK_SIZE);
		if(this->bottomMatrix[curRow][curCol] != NULL)
		{
			delete blocks;
			return true;
		}
	}
	delete blocks;
	// TODO: make the 'too high' value adjustable
	for(int i = 20; i > 0; i--)
	{
		for(int j = 0; j < colSize; j++)
		{
			if(this->bottomMatrix[i][j] != NULL)
			{
				printf("TOO HIGH TOP OUT\n");
				return true;
			}	
		}
	}	
	return false;
}

// Find contiguous rows of pieces and frees them from the board
// Moves the rest of the game board down to fill in the gaps
void
TetrisTable::FreeRows()
{
	printf("FREE CHECK\n");
	// TODO: still something wrong with this... keep monitoring it
	// keep track of which rows we are deleting
	std::vector<int> delRows(0);
	bool full = true;
	// search through every row, cataloging the full ones and deleting
	// everything in them
	for(int i = rowSize-1; i > 0; i--)
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
				printf("FULL ROW AT: %d\n", i);
				bottomMatrix[i][j]->Hide();
				bottomMatrix[i][j]->RemoveSelf();
				delete bottomMatrix[i][j];
				bottomMatrix[i][j] = NULL;
			}
		}
		full = true;
	}

	// scoring system
	// CLEAR:  1		2			3			4
	// POINTS: 40*(n+1) 100*(n+1)	300*(n+1)	1200*(n+1)
	// where n is the current level
	printf("DEL ROWS %p\n", delRows);
	if(delRows.size() <= 4 && delRows.size() > 0)
	{
		printf("HAVE ROWS TO DEL\n");
		// TODO: factor these things out into separate functions (classes?)
		// don't do this if it isn't array out of bounds, but that shouldn't happen
		this->score += this->scoreLevels[delRows.size()] * (this->level + 1);
		this->lines += delRows.size();
		// every level is at 10 lines
		// TODO: add different leveling schemes 
		if((this->lines + 1) % 10 == 0)
		{
			printf("NEXT LEVEL\n");
			this->level++;
		}
		// update text field
		dashUi->SetScore(this->score);
		dashUi->SetLines(this->lines);
		dashUi->SetLevel(this->level);
		printf("LINES: %d\n", this->lines);
	}
	
	// move all the blocks down to fill the gap
	// for every delRows move everything above it down by 1
	// then offset 1 from every other delRows
	int delRowsOffset = 0;
	for(int row = 0; row < delRows.size(); row++)
	{
		printf("MOVE ROW DOWN %d\n", row);
		for(int i = delRows[row]+delRowsOffset; i > 0; i--)
		{
			for(int j = 0; j < colSize; j++)
			{
				if(i-1 > 0)
				{
					bottomMatrix[i][j] = bottomMatrix[i-1][j];
					bottomMatrix[i-1][j] = NULL;
					if(bottomMatrix[i][j] != NULL)
					{
						bottomMatrix[i][j]->MoveBy(0,BLOCK_SIZE);	
					}
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
		if((col & COLLIDEL) == COLLIDEL)
		{
			if(bx < 0)
			{
				bx = 0;
			}
		} 
		if((col & COLLIDER) == COLLIDER)
		{
			if(bx > 0)
			{
				bx = 0;
			}	
		} 
		if((col & COLLIDE) == COLLIDE)
		{
			// collided on both sides, don't allow it to move 
			// horizontally at all
			bx = 0;
		} 
		if((col & STICK) == STICK)
		{
			// means we are gliding, don't let it move down at all
			by = 0;	
		}
		// check if this piece is moving beyond the screen, 
		// if so can't move sideways
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
		if(curFrame.left+bx >= this->colSize*BLOCK_SIZE || 
		   curFrame.right+bx < BLOCK_SIZE)
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
	int ret = 0;
	if(this->pc != NULL)
	{
		BRect *rects = this->pc->GetPos();
		for(int i = 0; i < this->pc->NUM_BLOCKS; i++)
		{
			BRect curRect = rects[i];
			int curCollide = CollidesBottomBlocks(curRect);
			// make the block stop if it hits the blocks at the bottom
			// or if it hits the bottom of the game board
			if(curRect.bottom > Bounds().Height()-BLOCK_SIZE 
			   || (curCollide & STICK) == STICK)
			{
				ret |= STICK;
				// before something sticks, allow it to move a little bit
				// but still have it collide
				// set the shiftTime to start now
				if(shiftTime == -1)
				{
					shiftTime = real_time_clock_usecs();	
				}
				
				// TODO: shift time should be adjustable and only happen
				// if the block is moved as it hits the ground
				if(real_time_clock_usecs() - shiftTime >= 100000)
				{
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
					shiftTime = -1;
					return STICK;
				}
			}
			// combine all of the collisions together (OR bit vectors)
			ret |= curCollide;
		}
	}
	return (CollisionType)ret;
}

TetrisTable::CollisionType 
TetrisTable::CollidesBottomBlocks(BRect rect)
{
	int row = (int)(rect.top/BLOCK_SIZE);
	int col = (int)(rect.left/BLOCK_SIZE);
	int ret = 0;	
	// bottom (STICK)
	if(row+1 < rowSize && this->bottomMatrix[row+1][col] != NULL)
	{
		ret |= STICK;
	}
	// COLLIDER
	if(col+1 < colSize && this->bottomMatrix[row][col+1] != NULL)
	{
		ret |= COLLIDER;	
	}
	// COLLIDEL
	if(col-1 >= 0 && this->bottomMatrix[row][col-1] != NULL)
	{
		ret |= COLLIDEL;
	}
	return (CollisionType)ret;
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
