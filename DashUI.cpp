#include "DashUI.h"

DashUI::DashUI(BRect dashBox, int previews)
	: BView(dashBox, "dashboard", B_FOLLOW_NONE, B_WILL_DRAW)
{
	AdoptSystemColors();
	this->previews = previews;
	this->previewRefs = new TetrisPiece*[previews];
	for(int i = 0; i < previews; i++)
	{
		this->previewRefs[i] = NULL;
	}
	float inset = 5;
	BRect bound = Bounds();
	float right = bound.right-inset;
	float left = bound.left+inset;
	float top = bound.top;
	BFont *sideFont = new BFont();
	sideFont->SetSize(15);
	// initialize all of the side boxes, keep the string views in an array
	// the boxes will remain for the entire program untouched so we don't
	// need their references
	BStringView **sideViews = new BStringView*[3];
	const char **names = new const char*[3];
	names[0] = "Score";
	names[1] = "Level";
	names[2] = "Lines";
	int height = 60;
	int nextTop = 0; // where to put the top of the new element
	for(int i = 0; i < 3; i++)
	{
		BoxView *b = new BoxView(BRect(left,top+5+(i*height),
									   right,top+55+(i*height)), 
	         					 NULL, B_FOLLOW_ALL, B_WILL_DRAW);
	    nextTop = top+55+(i*height);
		b->SetTitle(BString(names[i]));
		b->font->SetSize(15);
		AddChild(b);
		sideViews[i] = new BStringView(BRect(5,15,120,40), NULL, "0");
		sideViews[i]->SetFont(sideFont);
		b->AddChild(sideViews[i]);
	}
	this->scoreView = sideViews[0];
	this->levelView = sideViews[1];
	this->linesView = sideViews[2];
	delete sideViews;
	delete names;
	
	nextTop += 20;
	BoxView *b = new BoxView(BRect(left,nextTop,80,nextTop+220), 
	         				 NULL, B_FOLLOW_ALL, B_WILL_DRAW);
	this->blockView = new BView(BRect(5, 20, 65, 215),
								"preview", B_FOLLOW_ALL, B_WILL_DRAW);
	this->blockView->AdoptSystemColors();
	b->SetTitle(BString("Next"));
	b->font->SetSize(15);
	AddChild(b);
	b->AddChild(this->blockView);
	b = new BoxView(BRect(85,nextTop,right,nextTop+65), 
	                NULL, B_FOLLOW_ALL, B_WILL_DRAW);
	this->storedView = new BView(BRect(10,25,50,55),
	    						"preview", B_FOLLOW_ALL, B_WILL_DRAW);
	this->storedView->AdoptSystemColors();
	b->SetTitle(BString("Stored"));
	b->font->SetSize(15);
	AddChild(b);
	b->AddChild(this->storedView);
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

void 
DashUI::SetScore(long int newScore)
{
	char scoreStr[sizeof(long int)+1];
	sprintf(scoreStr, "%ld", newScore);
	this->scoreView->SetText(scoreStr);
}

void 
DashUI::SetLines(int newLines)
{
	char lineStr[sizeof(int)+1];
	sprintf(lineStr, "%d", newLines);
	this->linesView->SetText(lineStr);
}

void 
DashUI::SetLevel(int newLevel)
{
	char levelStr[sizeof(int)+1];
	sprintf(levelStr, "%d", newLevel);
	this->levelView->SetText(levelStr);
}

void
DashUI::SetStored(TetrisPiece *stored)
{
	if(this->stored != NULL)
	{
		this->stored->RemoveFromParent();
		BlockView **blocks = this->stored->GetBlocks();
		for(int i = 0; i < this->stored->NUM_BLOCKS; i++)
		{
			delete blocks[i];	
		}
		delete this->stored;
	}
	this->stored = new TetrisPiece(stored->type);
	this->stored->MoveTo(0,0);
	this->stored->ResizeTo(12);
	this->stored->AddToView(*this->storedView);
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
		cur->ResizeTo(12);
		// TODO: put the previews in a better spot?
		cur->MoveBy(0,offset);
		offset += cur->Height()+5;
		cur->AddToView(*this->blockView);
		this->previewRefs[i] = cur;
	}
}

void 
DashUI::SetBlockDeque(std::deque<TetrisPiece*> *newDeque)
{
	this->blockDeque = newDeque;
}
