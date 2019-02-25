#include "7Bag.h"

SevenBag::SevenBag()
	: RandomMethod()
{
	this->curPc = 7;
	for(int i = 0; i < 7; i++)
	{
		this->bag[i] = NULL;
	}
}

SevenBag::~SevenBag()
{
}

TetrisPiece *	
SevenBag::GetNextPiece()
{
	if(this->curPc == 7)
	{
		// populate bag with the 7 tetris pieces
		for(int i = 0; i < 7; i++)
		{
			this->bag[i] = new TetrisPiece((PieceType)i);
		}
		// shuffle the pieces
		const int swaps = 14;
		unsigned char *swapIdxs = GetRandomBytes(swaps*2);
		for(int i = 0; i < swaps; i++)
		{
			int idxOne = swapIdxs[i] % 7;
			int idxTwo = swapIdxs[i+swaps] % 7;
			TetrisPiece *store = this->bag[idxOne];
			this->bag[idxOne] = this->bag[idxTwo];
			this->bag[idxTwo] = store;
		}
		this->curPc = 0;
	}
	this->curPc++;
	return this->bag[this->curPc-1];
}
