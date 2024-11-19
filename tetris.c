#include "tetris.h"
#include <stdlib.h>
#include <string.h>

// 테트로미노 정의
tetris_location TETROMINOS[NUM_TETROMINOS][NUM_ORIENTATIONS][TETRIS] = {
    // I
    {{{0, 1}, {0, 2}, {0, 3}, {0, 4}}, {{0, 2}, {1, 2}, {2, 2}, {3, 2}},
     {{1, 0}, {1, 1}, {1, 2}, {1, 3}}, {{0, 1}, {1, 1}, {2, 1}, {3, 1}}},
    // J
    {{{0, 0}, {1, 0}, {1, 1}, {1, 2}}, {{0, 1}, {1, 1}, {2, 1}, {2, 0}},
     {{1, 0}, {1, 1}, {1, 2}, {0, 2}}, {{0, 0}, {0, 1}, {1, 1}, {2, 1}}},
    // L
    {{{0, 2}, {1, 0}, {1, 1}, {1, 2}}, {{0, 1}, {1, 1}, {2, 1}, {2, 2}},
     {{1, 0}, {1, 1}, {1, 2}, {0, 0}}, {{0, 0}, {1, 0}, {2, 0}, {2, 1}}},
    // O
    {{{0, 1}, {0, 2}, {1, 1}, {1, 2}}, {{0, 1}, {0, 2}, {1, 1}, {1, 2}},
     {{0, 1}, {0, 2}, {1, 1}, {1, 2}}, {{0, 1}, {0, 2}, {1, 1}, {1, 2}}},
    // S
    {{{0, 1}, {0, 2}, {1, 0}, {1, 1}}, {{0, 1}, {1, 1}, {1, 2}, {2, 2}},
     {{1, 1}, {1, 2}, {2, 0}, {2, 1}}, {{0, 0}, {1, 0}, {1, 1}, {2, 1}}},
    // T
    {{{0, 1}, {1, 0}, {1, 1}, {1, 2}}, {{0, 1}, {1, 1}, {1, 2}, {2, 1}},
     {{1, 0}, {1, 1}, {1, 2}, {2, 1}}, {{0, 1}, {1, 0}, {1, 1}, {2, 1}}},
    // Z
    {{{0, 0}, {0, 1}, {1, 1}, {1, 2}}, {{0, 2}, {1, 1}, {1, 2}, {2, 1}},
     {{1, 0}, {1, 1}, {2, 1}, {2, 2}}, {{0, 1}, {1, 0}, {1, 1}, {2, 0}}}
};

// 보드에서 특정 위치 가져오기
char tg_get(tetris_game *obj, int row, int col) {
    if (row < 0 || row >= obj->rows || col < 0 || col >= obj->cols) {
        return TC_EMPTY;
    }
    return obj->board[row * obj->cols + col];
}

// 보드에서 특정 위치 설정
void tg_set(tetris_game *obj, int row, int col, char value) {
    if (row < 0 || row >= obj->rows || col < 0 || col >= obj->cols) {
        return;
    }
    obj->board[row * obj->cols + col] = value;
}

// 블록이 현재 위치에 배치 가능한지 확인
bool tg_fits(tetris_game *obj, tetris_block block) {
    for (int i = 0; i < TETRIS; i++) {
        tetris_location cell = TETROMINOS[block.typ][block.ori][i];
        int r = block.loc.row + cell.row;
        int c = block.loc.col + cell.col;
        if (r < 0 || r >= obj->rows || c < 0 || c >= obj->cols || TC_IS_FILLED(tg_get(obj, r, c))) {
            return false;
        }
    }
    return true;
}

// 블록 회전
void tg_rotate(tetris_game *obj) {
    tetris_block temp = obj->falling;
    temp.ori = (temp.ori + 1) % NUM_ORIENTATIONS;
    if (tg_fits(obj, temp)) {
        obj->falling = temp;
    }
}

// 블록 배치
void tg_put(tetris_game *obj, tetris_block block) {
    for (int i = 0; i < TETRIS; i++) {
        tetris_location cell = TETROMINOS[block.typ][block.ori][i];
        tg_set(obj, block.loc.row + cell.row, block.loc.col + cell.col, TYPE_TO_CELL(block.typ));
    }
}

// 블록 제거
void tg_remove(tetris_game *obj, tetris_block block) {
    for (int i = 0; i < TETRIS; i++) {
        tetris_location cell = TETROMINOS[block.typ][block.ori][i];
        tg_set(obj, block.loc.row + cell.row, block.loc.col + cell.col, TC_EMPTY);
    }
}

// 한 줄이 가득 찼는지 확인
bool tg_line_full(tetris_game *obj, int row) {
    for (int col = 0; col < obj->cols; col++) {
        if (TC_IS_EMPTY(tg_get(obj, row, col))) {
            return false;
        }
    }
    return true;
}

// 한 줄 제거
void tg_clear_line(tetris_game *obj, int row) {
    for (int r = row; r > 0; r--) {
        for (int c = 0; c < obj->cols; c++) {
            tg_set(obj, r, c, tg_get(obj, r - 1, c));
        }
    }
    for (int c = 0; c < obj->cols; c++) {
        tg_set(obj, 0, c, TC_EMPTY);
    }
}

// 줄 삭제 및 점수 계산
void tg_check_lines(tetris_game *obj) {
    for (int row = 0; row < obj->rows; row++) {
        if (tg_line_full(obj, row)) {
            tg_clear_line(obj, row);
            obj->score += 100;
            obj->level = obj->score / 500 + 1;
            obj->gravity_delay = 500000 / obj->level;
        }
    }
}

// 보드 출력
void display_board(WINDOW *w, tetris_game *obj) {
    box(w, 0, 0);
    for (int i = 0; i < obj->rows; i++) {
        for (int j = 0; j < obj->cols; j++) {
            wmove(w, i + 1, j * COLS_PER_CELL + 1);
            if (TC_IS_FILLED(tg_get(obj, i, j))) {
                waddch(w, ' ' | A_REVERSE);
                waddch(w, ' ' | A_REVERSE);
            } else {
                waddch(w, ' ');
                waddch(w, ' ');
            }
        }
    }
    mvwprintw(w, 0, 2, "Score: %d Level: %d", obj->score, obj->level);
    wrefresh(w);
}

// 게임 초기화
void tg_init(tetris_game *obj, int rows, int cols) {
    obj->rows = rows;
    obj->cols = cols;
    obj->score = 0;
    obj->level = 1;
    obj->gravity_delay = 500000;
    obj->board = malloc(rows * cols);
    memset(obj->board, TC_EMPTY, rows * cols);
    obj->falling.typ = rand() % NUM_TETROMINOS;
    obj->falling.ori = 0;
    obj->falling.loc.row = 0;
    obj->falling.loc.col = cols / 2 - 2;

    obj->next.typ = rand() % NUM_TETROMINOS;
    obj->next.ori = 0;
    obj->next.loc.row = 0;
    obj->next.loc.col = cols / 2 - 2;
}
