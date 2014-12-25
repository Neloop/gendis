#include <ncurses.h>
#include <poll.h>

#include "../plugin_client.h"
#include "../plugin_server.h"

#define TRAIN_LENGTH 53
#define TRAIN_HEIGHT 10
#define COAL_LENGTH 30

static void my_mvprintw(int y, int x, char *str)
{
    int i;
    int length = strlen(str);
    int maxx = getmaxx(stdscr);

    for(i = 0; i < length; ++i)
    {
        if(x <= maxx){ mvaddch(y, x, str[i]); }
        ++x;
    }
}

static void print_train(int x, int y)
{
    my_mvprintw(y, x, "      ====        ________                ___________ ");
    my_mvprintw(++y, x, "  _D _|  |_______/        \\__I_I_____===__|_________| ");
    my_mvprintw(++y, x, "   |(_)---  |   H\\________/ |   |        =|___ ___|   ");
    my_mvprintw(++y, x, "   /     |  |   H  |  |     |   |         ||_| |_||   ");
    my_mvprintw(++y, x, "  |      |  |   H  |__--------------------| [___] |   ");
    my_mvprintw(++y, x, "  | ________|___H__/__|_____/[][]~\\_______|       |   ");
    my_mvprintw(++y, x, "  |/ |   |-----------I_____I [][] []  D   |=======|__ ");
}

static void print_wheel(int tick, int x, int y)
{
    if(tick == 1)
    {
        my_mvprintw(y, x, "__/ =| o |=-~~\\  /~~\\  /~~\\  /~~\\ ____Y___________|__ ");
        my_mvprintw(++y, x, " |/-=|___|=    ||    ||    ||    |_____/~\\___/        ");
        my_mvprintw(++y, x, "  \\_/      \\O=====O=====O=====O_/      \\_/            ");
    }
    else if(tick == 2)
    {
        my_mvprintw(y, x, "__/ =| o |=-~~\\  /~~\\  /~~\\  /~~\\ ____Y___________|__ ");
        my_mvprintw(++y, x, " |/-=|___|=O=====O=====O=====O   |_____/~\\___/        ");
        my_mvprintw(++y, x, "  \\_/      \\__/  \\__/  \\__/  \\__/      \\_/            ");
    }
    else if(tick == 3)
    {
        my_mvprintw(y, x, "__/ =| o |=-O=====O=====O=====O \\ ____Y___________|__ ");
        my_mvprintw(++y, x, " |/-=|___|=    ||    ||    ||    |_____/~\\___/        ");
        my_mvprintw(++y, x, "  \\_/      \\__/  \\__/  \\__/  \\__/      \\_/            ");
    }
    else if(tick == 4)
    {
        my_mvprintw(y, x, "__/ =| o |=-~O=====O=====O=====O\\ ____Y___________|__ ");
        my_mvprintw(++y, x, " |/-=|___|=    ||    ||    ||    |_____/~\\___/        ");
        my_mvprintw(++y, x, "  \\_/      \\__/  \\__/  \\__/  \\__/      \\_/            ");
    }
    else if(tick == 5)
    {
        my_mvprintw(y, x, "__/ =| o |=-~~\\  /~~\\  /~~\\  /~~\\ ____Y___________|__ ");
        my_mvprintw(++y, x, " |/-=|___|=   O=====O=====O=====O|_____/~\\___/        ");
        my_mvprintw(++y, x, "  \\_/      \\__/  \\__/  \\__/  \\__/      \\_/            ");
    }
    else
    {
        my_mvprintw(y, x, "__/ =| o |=-~~\\  /~~\\  /~~\\  /~~\\ ____Y___________|__ ");
        my_mvprintw(++y, x, " |/-=|___|=    ||    ||    ||    |_____/~\\___/        ");
        my_mvprintw(++y, x, "  \\_/      \\_O=====O=====O=====O/      \\_/            ");
    }
}

static void print_coal(int x, int y)
{
    my_mvprintw(y, x, "                              ");
    my_mvprintw(++y, x, "                              ");
    my_mvprintw(++y, x, "    _________________         ");
    my_mvprintw(++y, x, "   _|                \\_____A  ");
    my_mvprintw(++y, x, " =|                        |  ");
    my_mvprintw(++y, x, " -|                        |  ");
    my_mvprintw(++y, x, "__|________________________|_ ");
    my_mvprintw(++y, x, "|__________________________|_ ");
    my_mvprintw(++y, x, "   |_D__D__D_|  |_D__D__D_|   ");
    my_mvprintw(++y, x, "    \\_/   \\_/    \\_/   \\_/    ");
}

void run_client(network_info *con)
{
    int i;

    for(i = 0; i < con->count; ++i)
    {
        net_load_library(&con->remote_connections[i], "./libs/train.so");
    }

    for(i = 0; i < con->count; ++i)
    {
        int ok;

        net_write(&con->remote_connections[i], "go", 2);

        net_read(&con->remote_connections[i], &ok, sizeof(int));
    }

    for(i = 0; i < con->count; ++i)
    {
        net_write(&con->remote_connections[i], "end", 3);
    }

    return;
}

void run_server(connection_info *con)
{
    int x, y;
    int vertical;
    int horizontal;
    int tick = 0;
    char go[2];
    char end[3];
    int net_zero = htonl(0);

    initscr();

    clear();
    refresh();

    net_read(con, go, 2);

    x = getmaxx(stdscr);
    y = getmaxy(stdscr);
    vertical = (y - TRAIN_HEIGHT) / 2;
    horizontal = x;
    horizontal--;

    curs_set(0);

    while(horizontal + TRAIN_LENGTH + COAL_LENGTH >= 0)
    {
        clear();

        print_train(horizontal, vertical);
        print_wheel(6 - tick, horizontal, vertical + 7);

        print_coal(horizontal + TRAIN_LENGTH, vertical);

        if(horizontal == -1){ net_write(con, &net_zero, sizeof(int)); }

        tick = (tick + 1) % 6;
        horizontal--;

        refresh();

        poll(NULL, 0, 100);
    }

    net_read(con, end, 3);

    endwin();

    return;
}
