#include "RandomMethod.h"

#include <File.h>
#include <Directory.h>

RandomMethod::RandomMethod()
{
}

RandomMethod::~RandomMethod()
{
}

TetrisPiece	*
RandomMethod::GetNextPiece(void)
{
	// standard method, just random pieces
	unsigned char *type;
	type = GetRandomBytes(1);
	TetrisPiece *pc = new TetrisPiece((PieceType)(type[0]%7));
	delete type;
	return pc;
}


unsigned char *
RandomMethod::GetRandomBytes(int size)
{
	BFile random(new BDirectory("/dev"), "urandom", B_READ_ONLY);
	if(random.InitCheck() != B_OK)
	{
		printf("error with getting random numbers\n");
		// TODO: quit
	}
	unsigned char *bytes = new unsigned char[size];
	random.Read(bytes, size);
	return bytes;
}
