#include "DashUI.h"

DashUI::DashUI(BRect dashBox, int previews)
	: BView(dashBox, "dashboard", B_FOLLOW_ALL, B_WILL_DRAW)
{
	this->previews = previews;
	this->previewRefs = new TetrisPiece*[previews];
	for(int i = 0; i < previews; i++)
	{
		this->previewRefs[i] = NULL;
	}
	float inset = 20;
	BRect bound = Bounds();
	float right = bound.right-inset;
	float left = bound.left+inset;
	float top = bound.top;
	BStringView *label = new BStringView(BRect(left, top+10, right, top+30),
								NULL, "Score:");
	AddChild(label);
	label = new BStringView(BRect(left, top+50, right, top+70),
							NULL, "Level:");
	AddChild(label);
	label = new BStringView(BRect(left, top+90, right, top+110),
							NULL, "Lines:");
	AddChild(label);
	this->scoreView = new BStringView(BRect(left, top+30, right, top+50), 
								"score", "0");
	this->levelView = new BStringView(BRect(left, top+70, right, top+90), 
								"level", "0");
	this->linesView = new BStringView(BRect(left, top+110, right, top+130), 
								"lines", "0");
	this->blockView = new BView(BRect(left, top+150, right, top+480),
								"preview", B_FOLLOW_ALL, B_WILL_DRAW);
	AddChild(this->scoreView);
	AddChild(this->levelView);
	AddChild(this->linesView);
	AddChild(this->blockView);
}

DashUI::~DashUI(void)
{
	
}

void
DashUI::Draw(BRect rect)
{
	SetHighColor(0,0,0);
	StrokeRect(Bounds());	
}

void DashUI::SetScore(long int newScore)
{
	char scoreStr[sizeof(long int)+1];
	sprintf(scoreStr, "%ld", newScore);
	this->scoreView->SetText(scoreStr);
}

void DashUI::SetLines(int newLines)
{
	char lineStr[sizeof(int)+1];
	sprintf(lineStr, "%d", newLines);
	this->linesView->SetText(lineStr);
}

void DashUI::SetLevel(int newLevel)
{
	char levelStr[sizeof(int)+1];
	sprintf(levelStr, "%d", newLevel);
	this->levelView->SetText(levelStr);
}

void
DashUI::UpdatePreviews()
{
	int offset = 0;
	for(int i = 0; i < this->previews; i++)
	{
		TetrisPiece *curRef = this->previewRefs[i];
		if(curRef != NULL)
		{
			BlockView **blocks = curRef->GetBlocks();
			curRef->RemoveFromParent();
			for(int i = 0; i < curRef->NUM_BLOCKS; i++)
			{
				delete blocks[i];	
			}
			delete curRef;
			this->previewRefs[i] = NULL;
		}
		curRef = this->blockDeque->at(i);
		TetrisPiece *cur = new TetrisPiece(*curRef);
		// TODO: put the previews in a better spot?
		cur->MoveBy(0,offset);
		offset += 55;
		cur->AddToView(*this->blockView);
		this->previewRefs[i] = cur;
	}
}

void 
DashUI::SetBlockDeque(std::deque<TetrisPiece*> *newDeque)
{
	this->blockDeque = newDeque;
}
