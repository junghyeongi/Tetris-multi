#include "tetris.h"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main() {
    srand(time(NULL)); // 랜덤 초기화

    tetris_game tg;
    tg_init(&tg, 22, 10);

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    WINDOW *board = newwin(24, 22, 0, 0);

    bool running = true;

    while (running) {
        display_board(board, &tg);

        int ch = getch();
        switch (ch) {
            case 'q':
                running = false;
                break;
            case KEY_LEFT:
                tg_remove(&tg, tg.falling);
                tg.falling.loc.col--;
                if (!tg_fits(&tg, tg.falling)) {
                    tg.falling.loc.col++;
                }
                tg_put(&tg, tg.falling);
                break;
            case KEY_RIGHT:
                tg_remove(&tg, tg.falling);
                tg.falling.loc.col++;
                if (!tg_fits(&tg, tg.falling)) {
                    tg.falling.loc.col--;
                }
                tg_put(&tg, tg.falling);
                break;
            case KEY_UP:
                tg_remove(&tg, tg.falling);
                tg_rotate(&tg);
                tg_put(&tg, tg.falling);
                break;
        }

        usleep(tg.gravity_delay);
        tg_remove(&tg, tg.falling);
        tg.falling.loc.row++;
        if (!tg_fits(&tg, tg.falling)) {
            tg.falling.loc.row--;
            tg_put(&tg, tg.falling);
            tg_check_lines(&tg);
            tg.falling = tg.next;
            tg.next.typ = rand() % NUM_TETROMINOS;
            tg.next.ori = 0;
            tg.next.loc.row = 0;
            tg.next.loc.col = tg.cols / 2 - 2;
            if (!tg_fits(&tg, tg.falling)) {
                running = false;
            }
        }
        tg_put(&tg, tg.falling);
    }

    free(tg.board);
    endwin();
    return 0;
}
