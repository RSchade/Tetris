#ifndef TETRISTABLE_H
#define TETRISTABLE_H

#include "TetrisPiece.h"
#include "DashUI.h"
#include "BlockView.h"
#include "RandomMethod.h"

#include <SupportDefs.h>
#include <View.h>
#include <Point.h>
#include <Control.h>
#include <InterfaceDefs.h>
#include <Locker.h>
#include <deque>

class TetrisTable : public BView {
public:
				TetrisTable(DashUI *ui, int rowSize, int colSize);
				~TetrisTable(void);
	void		Draw(BRect rect);
	void    	KeyDown(const char *bytes, int32 numBytes);
	void    	Tick();
	void    	Pause();
	void    	Start();
	int			level;
	int			lines;
	long int	score;
	int			G;  // TODO: use the gravity constant for stuff (maybe set it?)
	
private:
	enum CollisionType
	{
		NONE = 0,
		BELOWL = 16,
		BELOWR = 32,
		STICK = 8,
		BELOW = 4,
		COLLIDEL = 1,
		COLLIDER = 2,
		COLLIDE = 3
	};	
	void						MoveActive(int bx, int by);
	void						RotateActive(PieceRot rot);
	void						NewPiece();
	void						FreeRows();
	void						StorePiece();
	void						HardDrop();
	bool						BeyondScreen(int bx);
	bool						TopOut(TetrisPiece *next);
	void						UpdateShadow();
	TetrisPiece					*GetShadow();
	TetrisPiece					*shadow = NULL;
	TetrisPiece					*storage = NULL;
	int							sinceStore = 100;
	BLocker						moveLock;
	BPoint						GetSpawnLoc(TetrisPiece *piece);
	CollisionType				CollidesBottomBlocks(BRect rect);
	CollisionType				CheckCollision(int bx);
	RandomMethod				*randomizer;
	BView						*block;
	thread_id					time_thread;
	TetrisPiece					*pc;
	BlockView					***bottomMatrix;
	DashUI						*dashUi;
	std::deque<TetrisPiece*>	nextBlocks;
	bigtime_t					shiftTime;
	int 						scoreLevels[5] = {0, 40, 100, 300, 1200};
	int							gray[3] = {128,128,128};
	int							rowSize;
	int							colSize;
};


#endif // TETRISTABLE_H
