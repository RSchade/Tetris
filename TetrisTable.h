#ifndef TETRISTABLE_H
#define TETRISTABLE_H

#include "TetrisPiece.h"
#include "BlockView.h"

#include <SupportDefs.h>
#include <View.h>
#include <Point.h>
#include <Control.h>
#include <InterfaceDefs.h>
#include <vector>

class TetrisTable : public BView {
public:
			TetrisTable(void);
			~TetrisTable(void);
	void	Draw(BRect rect);
	void    KeyDown(const char *bytes, int32 numBytes);
	void    Tick();
	void    Pause();
	void    Start();
private:
	enum CollisionType
	{
		NONE = 0,
		STICK = 4,
		COLLIDEL = 1,
		COLLIDER = 2,
		COLLIDE = 3
	};	
	void					MoveActive(int bx, int by);
	void					RotateActive(PieceRot rot);
	void					NewPiece();
	CollisionType			CollidesBottomBlocks(BRect rect);
	CollisionType			CheckCollision();
	BView					*block;
	thread_id				time_thread;
	TetrisPiece				*pc;
	std::vector<BlockView*>	bottomBlocks;
};


#endif // TETRISTABLE_H
