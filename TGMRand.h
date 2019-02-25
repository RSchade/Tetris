#ifndef TGMRAND_H
#define TGMRAND_H

#include "RandomMethod.h"
#include "TetrisPiece.h"

#include <SupportDefs.h>


class TGMRand : public RandomMethod {
public:
					TGMRand(int recent, int tries);
					~TGMRand(void);
	TetrisPiece		*GetNextPiece(void);
private:
	bool			newGame;
	int 			recent;
	int 			tries;
	PieceType		*history;
	int				historyIdx;
};


#endif // TGMRAND_H
