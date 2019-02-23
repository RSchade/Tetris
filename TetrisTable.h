#ifndef TETRISTABLE_H
#define TETRISTABLE_H

#include "TetrisPiece.h"
#include "BlockView.h"

#include <SupportDefs.h>
#include <View.h>
#include <Point.h>
#include <Control.h>
#include <InterfaceDefs.h>
#include <StringView.h>
#include <queue>

const int WIDTH = 400;
const int HEIGHT = 400;
const int rowSize = HEIGHT/BLOCK_SIZE;
const int colSize = WIDTH/BLOCK_SIZE;

class TetrisTable : public BView {
public:
				TetrisTable(BStringView *scoreView);
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
	CollisionType				CollidesBottomBlocks(BRect rect);
	CollisionType				CheckCollision();
	BView						*block;
	thread_id					time_thread;
	TetrisPiece					*pc;
	BlockView					*bottomMatrix[rowSize][colSize];
	std::queue<TetrisPiece*>	nextBlocks;
	bigtime_t					shiftTime;
	int 						scoreLevels[5] = {0, 40, 100, 300, 1200};
	BStringView					*scoreView;
	enum
	{
		M_CLEARED_ROW = 'crow'
	};
};


#endif // TETRISTABLE_H
