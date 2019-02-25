#include "TGMRand.h"

TGMRand::TGMRand(int recent, int tries)
	: RandomMethod()
{
	this->recent = recent;
	this->tries = tries;
	this->historyIdx = 0;
	this->newGame = true;
	this->history = new PieceType[recent];
	
	// TODO: customize this?
	// history is initialized to a fixed state
	// All Z
	for(int i = 0; i < recent; i++)
	{
		this->history[i] = ZPOLY;
	}
}

TGMRand::~TGMRand()
{
}

TetrisPiece	*
TGMRand::GetNextPiece(void)
{
	// keep a history of a few recent pieces
	// try to choose a random one not in history
	// try a certain amount of times to generate one
	// not in the history
	
	unsigned char *rand;
	bool repeated = false;
	PieceType typ;
	rand = GetRandomBytes(tries);
	for(int i = 0; i < this->tries; i++)
	{
		typ = (PieceType)(rand[i] % 7);
		for(int j = 0; j < this->recent; j++)
		{
			if(this->history[j] == typ)
			{
				repeated = true;
				break;
			}
		}
		if(!repeated)
		{
			break;
		}
	}
	delete rand;
	TetrisPiece *pc = new TetrisPiece(typ);
	this->history[this->historyIdx] = typ;
	this->historyIdx = (this->historyIdx + 1) % this->recent;
	return pc;
}
