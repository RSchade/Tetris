#include "MainWindow.h"

#include <Application.h>
#include <Menu.h>
#include <MenuItem.h>
#include <View.h>
#include <stdio.h>
#include <Message.h>

MainWindow::MainWindow(void)
	:	BWindow(BRect(100,100,600,700),"Main Window",B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS)
{
	BStringView *scoreView = new BStringView(BRect(450, 25, 475, 50), "score", "0");
	AddChild(scoreView);
	
	// per tetris spec, 40 rows, 10 columns, 20 rows, 10 columns visible
	this->table = new TetrisTable(scoreView, 40, 10);
	// move up to only have part of it visible
	this->table->MoveTo(BPoint(0, -20*BLOCK_SIZE));
	AddChild(this->table);
	table->MakeFocus(true);
	
	// TODO: Make UI with new game, pause game, etc...
	// TODO: High score system
	
	// simulate clicking new game
	PostMessage(new BMessage(M_NEW_GAME));
}


void
MainWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_NEW_GAME:
		{
			printf("START NEW GAME\n");
			table->Start();
			break;	
		}
		case M_PAUSE_GAME:
		{
			printf("PAUSE GAME\n");
			table->Pause();
			break;	
		}
		default:
		{
			BWindow::MessageReceived(msg);
			break;
		}
	}
}


bool
MainWindow::QuitRequested(void)
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}
