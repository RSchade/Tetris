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

int32
tetris_clock(void *params)
{
	TetrisTable *table = (TetrisTable *)params;
	while(1) {
		table->Tick();
		// TODO: different speed curves using G
		double times = pow((0.8 - ((table->level-1)*0.007)), (table->level-1));
		//long int waitTime = adjustedG * table->G;
		snooze((int)(times*1000000));
	}
	return 0;
}

// width and height are in blocks
TetrisTable::TetrisTable(DashUI *ui, int rowSize, int colSize)
 :	BView(BRect(0,0,colSize*BLOCK_SIZE,rowSize*BLOCK_SIZE), 
    "tetristable", B_FOLLOW_NONE, B_WILL_DRAW)
{	
	// the standard rate of falling, 1 cell per 'frame'
	// 'frames' are 60 times per second, so 0.01666
	// in microseconds
	this->G = 16666;
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
		throw "no more threads";
	}
	NewPiece();
}

TetrisTable::~TetrisTable(void) 
{
}

void 
TetrisTable::Draw(BRect rect) 
{
	//SetHighColor(0,0,0);
	//StrokeRect(Bounds());
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
		if(this->pc != NULL)
		{
			// Lower the active piece
			MoveActive(0,BLOCK_SIZE);
		} 
		
		// need this, state can change after MoveActive
		if(this->pc == NULL) {
			// increase store counter, if this is high enough 
			// a piece can be retrieved
			sinceStore++;
			// Check for rows to free at the bottom
			FreeRows();
			// Check for lose state
			if(TopOut(this->nextBlocks.front()))
			{
				printf("GAME LOST\n");
				Pause();
			} else
			{
				// Create a new piece at the top
				NewPiece();
			}
		}
		
		UpdateShadow();
		
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
		if((this->lines + 1) % 1 == 0)
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
	if(this->moveLock.Lock())
	{
		CollisionType col = CheckCollision(bx);
		if(this->pc != NULL) 
		{
			if((col & COLLIDEL) == COLLIDEL)
			{
				printf("L %d\n", bx);
				if(bx < 0)
				{
					bx = 0;
				}
			} 
			if((col & COLLIDER) == COLLIDER)
			{
				printf("R %d\n", bx);
				if(bx > 0)
				{
					bx = 0;
				}	
			} 
			if((col & COLLIDE) == COLLIDE)
			{
				printf("B\n");
				// collided on both sides, don't allow it to move 
				// horizontally at all
				bx = 0;
			} 
			if((col & STICK) == STICK)
			{
				// means we are on top of another piece, can't move down
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
		this->moveLock.Unlock();
	}
}

void
TetrisTable::UpdateShadow()
{
	// create the shadow
	if(this->shadow != NULL)
	{
		this->shadow->RemoveFromParent();
		BlockView **blocks = this->shadow->GetBlocks();
		for(int i = 0; i < this->shadow->NUM_BLOCKS; i++)
		{
			delete blocks[i];
		}
		delete this->shadow;
	}
	this->shadow = GetShadow();
	if(this->shadow != NULL)
	{
		this->shadow->AddToView(*this);
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

// Gets the shadow piece for the current active piece, colored clear
TetrisPiece *
TetrisTable::GetShadow()
{
	if(this->pc != NULL)
	{
		int lowest = -1;
		TetrisPiece *shadow = new TetrisPiece(*this->pc);
		shadow->SetColor(gray);
		while(lowest == -1 || lowest < Bounds().Height()-BLOCK_SIZE)
		{
			BRect *rects = shadow->GetPos();
			for(int i = 0; i < shadow->NUM_BLOCKS; i++)
			{
				BRect curRect = rects[i];
				int curCollide = CollidesBottomBlocks(curRect);
				if(lowest == -1 || curRect.bottom > lowest)
				{
					lowest = (int)curRect.bottom;
				}
				if((curCollide & BELOW) == BELOW)
				{
					return shadow;	
				}
			}
			shadow->MoveBy(0,BLOCK_SIZE);
		}
		if(lowest > Bounds().Height()-BLOCK_SIZE)
		{
			shadow->MoveBy(0,-BLOCK_SIZE);
		}
		return shadow;
	}
	return NULL;
}

// Collides with the bottom/floor if the current block touches it
// makes the current block become apart of the floor
TetrisTable::CollisionType
TetrisTable::CheckCollision(int bx)
{	
	int ret = 0;
	bool resetShiftTime = true;
	if(this->pc != NULL)
	{
		BRect *rects = this->pc->GetPos();
		for(int i = 0; i < this->pc->NUM_BLOCKS; i++)
		{
			BRect curRect = rects[i];
			int curCollide = CollidesBottomBlocks(curRect);
			// make the block stop if it hits the blocks at the bottom
			// or if it hits the bottom of the game board
			if(curRect.bottom >= Bounds().Height()-BLOCK_SIZE 
			   || (curCollide & STICK) == STICK)
			{
				ret |= STICK;
				if((curCollide & BELOW) == BELOW || 
					curRect.bottom >= Bounds().Height())
				{
					resetShiftTime = false;
				}
			}
			// combine all of the collisions together (OR bit vectors)
			ret |= curCollide;
		}
		// before something sticks, allow it to move a little bit
		// but still have it collide
		// set the shiftTime to start now
		if((ret & STICK) == STICK && shiftTime == -1)
		{
			printf("START TIMER\n");
			resetShiftTime = false;
			shiftTime = real_time_clock_usecs();
			printf("BELOW: %d\n", ret & BELOW);
			// don't move down if there's something below or
			// if it will cause a left or right collision
			bool moveRight = bx > 0;
			bool moveLeft = bx < 0;
			printf("MR %d ML %d\n", moveRight, moveLeft);
			if((ret & BELOW) == 0 &&
			   (!BELOWL || !moveLeft) &&
			   (!BELOWR || !moveRight))
			{
				printf("MOVE DOWN\n");
				// move last bit of the way to the piece
				this->pc->MoveBy(0,BLOCK_SIZE);
			}
		}
		if(resetShiftTime)
		{
			printf("RESET SHIFT TIMER\n");
			shiftTime = -1;
		}
		// if we have been 'shifting' along a surface and the timers
		// ended successfully, stop the block here
		if(shiftTime != -1 && real_time_clock_usecs() - shiftTime >= 300000)
		{
			printf("END TIMER\n");
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
	return (CollisionType)ret;
}

TetrisTable::CollisionType 
TetrisTable::CollidesBottomBlocks(BRect rect)
{
	int row = (int)(rect.top/BLOCK_SIZE);
	int col = (int)(rect.left/BLOCK_SIZE);
	int ret = 0;	
	// block exists below
	if(row+1 < rowSize)
	{
		if(this->bottomMatrix[row+1][col] != NULL)
		{
			printf("BELOW\n");
			ret |= BELOW;
			ret |= STICK;
		}
		// if a block exists diagonally below
		if(col+1 < colSize && this->bottomMatrix[row+1][col+1] != NULL) 
		{
			ret |= BELOWR;
		}
		if(col-1 >= 0 && this->bottomMatrix[row+1][col-1] != NULL)
		{
			ret |= BELOWL;
		}
	}
	// bottom (1 or 2 blocks) (STICK)
	if(row+2 < rowSize && this->bottomMatrix[row+2][col] != NULL)
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
	// TODO: T-Spin
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

// stores the piece, or releases it if the piece hasn't been stored yet
void
TetrisTable::StorePiece()
{
	if(sinceStore > 1)
	{
		// take piece out of storage
		TetrisPiece *toReplace = this->storage;
		// put piece into storage
		this->storage = this->pc;
		this->storage->RemoveFromParent();
		// update the ui
		this->dashUi->SetStored(this->storage);
		this->pc = toReplace;
		if(this->pc != NULL)
		{
			// TODO: kind of repeated code?
			BPoint spawn = GetSpawnLoc(toReplace);
			this->pc->MoveTo(spawn.x,spawn.y);
			toReplace->AddToView(*this);
		}
		Tick();
		sinceStore = 0;
	}
}

void
TetrisTable::HardDrop()
{
	if(this->pc != NULL)
	{
		// add the shadow's pieces to the background
		TetrisPiece *shadow = GetShadow();
		shadow->SetColor(this->pc->color);
		shadow->AddToView(*this);
		this->pc->RemoveFromParent();
		BlockView **blocks = this->pc->GetBlocks();
		BlockView **newBlocks = shadow->GetBlocks();
		for(int i = 0; i < this->pc->NUM_BLOCKS; i++)
		{
			delete blocks[i];
			BlockView *block = newBlocks[i];
			BRect f = block->Frame();
			int col = f.LeftTop().x / BLOCK_SIZE;
			int row = f.LeftTop().y / BLOCK_SIZE;
			this->bottomMatrix[row][col] = block;
		}
		delete this->pc;
		delete shadow;
		this->pc = NULL;
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
				printf("DOWN\n");
				MoveActive(0,BLOCK_SIZE);
				break;
			case B_LEFT_ARROW:
				printf("LEFT\n");
				MoveActive(-BLOCK_SIZE,0);
				break;
			case B_RIGHT_ARROW:
				printf("RIGHT\n");
				MoveActive(BLOCK_SIZE,0);
				break;
			case 'z':
				RotateActive(BACKWARD);
				break;
			case 'x':
				RotateActive(FORWARD);
				break;
			case 'c':
				StorePiece();
				break;
			case B_SPACE:
				HardDrop();
				break;
		}	
	}
	
	UpdateShadow();
}
