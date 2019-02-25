#ifndef RANDOMMETHOD_H
#define RANDOMMETHOD_H

#include "TetrisPiece.h"

#include <SupportDefs.h>

class RandomMethod {
public:
						RandomMethod(void);
						~RandomMethod(void);
	unsigned char		*GetRandomBytes(int size);
	virtual	TetrisPiece	*GetNextPiece(void);
private:

};


#endif // RANDOMMETHOD_H
