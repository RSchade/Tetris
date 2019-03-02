#include "MainWindow.h"
#include "DashUI.h"
#include "TableBorder.h"

#include <Application.h>
#include <Menu.h>
#include <MenuItem.h>
#include <View.h>
#include <stdio.h>
#include <Message.h>

MainWindow::MainWindow(void)
	:	BWindow(BRect(100,100,505,607),"Main Window",
			    B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS)
{	
	DashUI *ui = new DashUI(BRect((BLOCK_SIZE*10)+11, 0, 405, 506), 6);
	AddChild(ui);
	
	// per tetris spec, 40 rows, 10 columns, 20 rows, 10 columns visible
	this->table = new TetrisTable(ui, 40, 10);
	// move up to only have part of it visible
	this->table->MoveTo(BPoint(5, -20*BLOCK_SIZE));
	AddChild(this->table);
	table->MakeFocus(true);
	// make a stroke around the visible game board
	BView *tableStroke = 
		new TableBorder(BRect(0,0,(BLOCK_SIZE*10)+11,(BLOCK_SIZE*20)+6), 
						NULL, B_FOLLOW_NONE, B_WILL_DRAW);
	AddChild(tableStroke);
	
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
