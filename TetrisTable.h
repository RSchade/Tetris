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
#include <StringView.h>
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
	
private:
	enum CollisionType
	{
		NONE = 0,
		STICK = 8,
		COLLIDEL = 1,
		COLLIDER = 2,
		COLLIDE = 3
	};	
	void						MoveActive(int bx, int by);
	void						RotateActive(PieceRot rot);
	void						NewPiece();
	void						FreeRows();
	bool						BeyondScreen(int bx);
	bool						TopOut(TetrisPiece *next);
	BPoint						GetSpawnLoc(TetrisPiece *piece);
	CollisionType				CollidesBottomBlocks(BRect rect);
	CollisionType				CheckCollision();
	RandomMethod				*randomizer;
	BView						*block;
	thread_id					time_thread;
	TetrisPiece					*pc;
	BlockView					***bottomMatrix;
	DashUI						*dashUi;
	std::deque<TetrisPiece*>	nextBlocks;
	bigtime_t					shiftTime;
	int 						scoreLevels[5] = {0, 40, 100, 300, 1200};
	int							rowSize;
	int							colSize;
};


#endif // TETRISTABLE_H
