#ifndef TETRIS_H
#define TETRIS_H

#include <stdbool.h>
#include <ncurses.h>

// 상수 정의
#define COLS_PER_CELL 2
#define TETRIS 4
#define NUM_TETROMINOS 7
#define NUM_ORIENTATIONS 4
#define TC_EMPTY 0
#define TC_IS_EMPTY(x) ((x) == TC_EMPTY)
#define TC_IS_FILLED(x) (!TC_IS_EMPTY(x))
#define TYPE_TO_CELL(type) ((type) + 1)

// 구조체 정의
typedef struct {
    int row, col;
} tetris_location;

typedef struct {
    int typ;  // 블록 타입
    int ori;  // 회전 상태
    tetris_location loc;  // 위치
} tetris_block;

typedef struct {
    int rows, cols;
    char *board;
    int score;
    int level;
    int gravity_delay;
    tetris_block falling, next;
} tetris_game;

// 함수 프로토타입
void tg_init(tetris_game *obj, int rows, int cols);
char tg_get(tetris_game *obj, int row, int col);
void tg_set(tetris_game *obj, int row, int col, char value);
bool tg_fits(tetris_game *obj, tetris_block block);
void tg_rotate(tetris_game *obj);
void tg_put(tetris_game *obj, tetris_block block);
void tg_remove(tetris_game *obj, tetris_block block);
bool tg_line_full(tetris_game *obj, int row);
void tg_clear_line(tetris_game *obj, int row);
void tg_check_lines(tetris_game *obj);
void display_board(WINDOW *w, tetris_game *obj);

#endif // TETRIS_H
