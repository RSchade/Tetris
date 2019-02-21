#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "TetrisTable.h"

#include <Window.h>
#include <MenuBar.h>

class MainWindow : public BWindow
{
public:
						MainWindow(void);
			void		MessageReceived(BMessage *msg);
			bool		QuitRequested(void);
			
private:
			BMenuBar	*fMenuBar;
			TetrisTable *table;
			enum
			{
				M_NEW_GAME = 'ngam',
				M_PAUSE_GAME = 'pgam'
			};
};

#endif
