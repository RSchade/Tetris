#ifndef SEVENBAG_H
#define SEVENBAG_H

#include "RandomMethod.h"
#include "TetrisPiece.h"

#include <SupportDefs.h>


class SevenBag : public RandomMethod {
public:
					SevenBag(void);
					~SevenBag(void);
	TetrisPiece		*GetNextPiece();
private:
	TetrisPiece		*bag[7];
	int 			curPc;
};


#endif // SEVENBAG_H
