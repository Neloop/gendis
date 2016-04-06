/*
 * CMD line image of train was taken from sl command written by Toyoda Masashi
 * 
 * License:
 *   Copyright 1993,1998,2014 Toyoda Masashi (mtoyoda@acm.org)
 *   
 *   Everyone is permitted to do anything on this program including copying,
 *   modifying, and improving, unless you try to pretend that you wrote it.
 *   i.e., the above copyright notice has to appear in all copies.
 *   THE AUTHOR DISCLAIMS ANY RESPONSIBILITY WITH REGARD TO THIS SOFTWARE.
 */

#include <ncurses.h>
#include <poll.h>

#include "../plugin_client.h"
#include "../plugin_server.h"

#define	TRAIN_LENGTH	53
#define	TRAIN_HEIGHT	10
#define	COAL_LENGTH	30

static void my_mvprintw(int y, int x, char *str)
{
	int i;
	int length = strlen(str);
	int maxx = getmaxx(stdscr);

	for (i = 0; i < length; ++i) {
		if (x <= maxx) { mvaddch(y, x, str[i]); }
		++x;
	}
}

/* Copy from original sl and slightly modified */
static void print_smoke(int x, int y)
#define SMOKEPTNS 16
{
	static struct smokes {
	int y, x;
	int ptrn, kind;
	} S[1000];
	static int sum = 0;
	static char *Smoke[2][SMOKEPTNS] =
		{ { "(	 )", "(	   )", "(	 )", "(	  )", "(  )",
			"(	)" , "( )"	 , "( )"   , "()"	, "()"	,
			"O"	   , "O"	 , "O"	   , "O"	, "O"	,
			" " },
		   { "(@@@)", "(@@@@)", "(@@@@)", "(@@@)", "(@@)",
			"(@@)" , "(@)"	 , "(@)"   , "@@"	, "@@"	,
			"@"	   , "@"	 , "@"	   , "@"	, "@"	,
			" " } };

	static int dy[SMOKEPTNS] = { 1,	 1, 1, 1, 0, 0, 0, 0, 0, 0,
		0,	 0, 0, 0, 0, 0 };
	static int dx[SMOKEPTNS] = {-2, -1, 0, 1, 1, 1, 1, 1, 2, 2,
		2,	 2, 2, 3, 3, 3 };
	int i;

	if ((x % 1) == 0) {
		for (i = 0; i < sum; ++i) {
			S[i].y	  -= dy[S[i].ptrn];
			S[i].x	  += dx[S[i].ptrn] + 3;
			S[i].ptrn += (S[i].ptrn < SMOKEPTNS - 1) ? 1 : 0;
			my_mvprintw(S[i].y, S[i].x,
				Smoke[S[i].kind][S[i].ptrn]);
		}
		my_mvprintw(y, x, Smoke[sum % 2][0]);
		S[sum].y = y;	 S[sum].x = x;
		S[sum].ptrn = 0; S[sum].kind = sum % 2;
		sum ++;
	}
}

static void print_train(int x, int y)
{
	my_mvprintw(y, x, "		 ====		 ________				 ___________ ");
	my_mvprintw(++y, x, "  _D _|  |_______/		   \\__I_I_____===__|_________| ");
	my_mvprintw(++y, x, "	|(_)---	 |	 H\\________/ |	  |		   =|___ ___|	");
	my_mvprintw(++y, x, "	/	  |	 |	 H	|  |	 |	 |		   ||_| |_||   ");
	my_mvprintw(++y, x, "  |	  |	 |	 H	|__--------------------| [___] |   ");
	my_mvprintw(++y, x, "  | ________|___H__/__|_____/[][]~\\_______|		|	");
	my_mvprintw(++y, x, "  |/ |	  |-----------I_____I [][] []  D   |=======|__ ");
}

static void print_wheel(int tick, int x, int y)
{
	if (tick == 1) {
		my_mvprintw(y, x, "__/ =| o |=-~~\\	 /~~\\	/~~\\  /~~\\ ____Y___________|__ ");
		my_mvprintw(++y, x, " |/-=|___|=	||	  ||	||	  |_____/~\\___/		");
		my_mvprintw(++y, x, "  \\_/		 \\O=====O=====O=====O_/	  \\_/			  ");
	} else if (tick == 2) {
		my_mvprintw(y, x, "__/ =| o |=-~~\\	 /~~\\	/~~\\  /~~\\ ____Y___________|__ ");
		my_mvprintw(++y, x, " |/-=|___|=O=====O=====O=====O	  |_____/~\\___/		");
		my_mvprintw(++y, x, "  \\_/		 \\__/	\\__/  \\__/  \\__/		 \\_/			 ");
	} else if (tick == 3) {
		my_mvprintw(y, x, "__/ =| o |=-O=====O=====O=====O \\ ____Y___________|__ ");
		my_mvprintw(++y, x, " |/-=|___|=	||	  ||	||	  |_____/~\\___/		");
		my_mvprintw(++y, x, "  \\_/		 \\__/	\\__/  \\__/  \\__/		 \\_/			 ");
	} else if (tick == 4) {
		my_mvprintw(y, x, "__/ =| o |=-~O=====O=====O=====O\\ ____Y___________|__ ");
		my_mvprintw(++y, x, " |/-=|___|=	||	  ||	||	  |_____/~\\___/		");
		my_mvprintw(++y, x, "  \\_/		 \\__/	\\__/  \\__/  \\__/		 \\_/			 ");
	} else if (tick == 5) {
		my_mvprintw(y, x, "__/ =| o |=-~~\\	 /~~\\	/~~\\  /~~\\ ____Y___________|__ ");
		my_mvprintw(++y, x, " |/-=|___|=   O=====O=====O=====O|_____/~\\___/		");
		my_mvprintw(++y, x, "  \\_/		 \\__/	\\__/  \\__/  \\__/		 \\_/			 ");
	} else {
		my_mvprintw(y, x, "__/ =| o |=-~~\\	 /~~\\	/~~\\  /~~\\ ____Y___________|__ ");
		my_mvprintw(++y, x, " |/-=|___|=	||	  ||	||	  |_____/~\\___/		");
		my_mvprintw(++y, x, "  \\_/		 \\_O=====O=====O=====O/	  \\_/			  ");
	}
}

static void print_coal(int x, int y)
{
	my_mvprintw(y, x, "								 ");
	my_mvprintw(++y, x, "							   ");
	my_mvprintw(++y, x, "	 _________________		   ");
	my_mvprintw(++y, x, "	_|				  \\_____A	");
	my_mvprintw(++y, x, " =|						|  ");
	my_mvprintw(++y, x, " -|						|  ");
	my_mvprintw(++y, x, "__|________________________|_ ");
	my_mvprintw(++y, x, "|__________________________|_ ");
	my_mvprintw(++y, x, "	|_D__D__D_|	 |_D__D__D_|   ");
	my_mvprintw(++y, x, "	 \\_/	\\_/	\\_/   \\_/	   ");
}

void
run_client(network_info *con)
{
	int i;

	for (i = 0; i < con->count; ++i) {
		net_load_library(&con->remote_connections[i],
			"./libs/libtrain.so");
	}

	for (i = 0; i < con->count; ++i) {
		int cont;

		net_write(&con->remote_connections[i], "go", 2);

		net_read(&con->remote_connections[i], &cont, sizeof (int));
	}

	for (i = 0; i < con->count; ++i) {
		int ok;

		net_read(&con->remote_connections[i], &ok, sizeof (int));
	}

	for (i = 0; i < con->count; ++i) {
		net_write(&con->remote_connections[i], "end", 3);
	}
}

void
run_server(connection_info *con)
{
	int x, y;
	int vertical;
	int horizontal;
	int tick = 0;
	char go[2];
	char end[3];
	int net_zero = htonl(0);

	initscr();

	curs_set(0);

	clear();
	refresh();

	net_read(con, &go, 2);

	x = getmaxx(stdscr);
	y = getmaxy(stdscr);
	vertical = (y - TRAIN_HEIGHT) / 2;
	horizontal = x;
	horizontal--;

	while (horizontal + TRAIN_LENGTH + COAL_LENGTH >= 0) {
		clear();

		print_train(horizontal, vertical);
		print_wheel(6 - tick, horizontal, vertical + 7);
		print_smoke(horizontal + 6, vertical - 1);

		print_coal(horizontal + TRAIN_LENGTH, vertical);

		if (horizontal == -1) {
			net_write(con, &net_zero, sizeof (int));
		}

		tick = (tick + 1) % 6;
		horizontal--;

		refresh();

		poll(NULL, 0, 100);
	}

	clear();

	net_write(con, &net_zero, sizeof (int));

	net_read(con, &end, 3);

	endwin();
}
