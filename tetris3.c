
#include <ncurses.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define WIDTH 10       // 테트리스 보드의 너비
#define HEIGHT 20      // 테트리스 보드의 높이
#define TETRIS 4       // 각 테트로미노는 4개의 셀로 구성

// 테트로미노 모양 정의
// 테트로미노 모양 정의
int TETROMINOS[7][4][4][2] = {
    // I
    {{{1, 0}, {1, 1}, {1, 2}, {1, 3}}, {{0, 2}, {1, 2}, {2, 2}, {3, 2}},
     {{2, 0}, {2, 1}, {2, 2}, {2, 3}}, {{0, 1}, {1, 1}, {2, 1}, {3, 1}}},
    // J
    {{{0, 0}, {1, 0}, {1, 1}, {1, 2}}, {{0, 1}, {1, 1}, {2, 1}, {2, 2}},
     {{1, 0}, {1, 1}, {1, 2}, {2, 0}}, {{0, 0}, {0, 1}, {1, 1}, {2, 1}}},
    // L
    {{{0, 2}, {1, 0}, {1, 1}, {1, 2}}, {{0, 1}, {1, 1}, {2, 1}, {2, 0}},
     {{1, 0}, {1, 1}, {1, 2}, {2, 2}}, {{0, 1}, {1, 1}, {2, 0}, {2, 1}}},
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

// 블록 구조체 정의
typedef struct {
    int x, y;           // 블록 위치
    int shape, rotation; // 블록 모양과 회전 상태
} Block;

// 전역 변수
int field[HEIGHT][WIDTH] = {0}; // 필드 배열
int score = 0;                  // 점수
Block current_block;            // 현재 블록
Block next_block;               // 다음 블록

// 함수 선언
void init_game();
void draw_field();
void draw_block(Block block);
void generate_new_block();
bool check_collision(Block block, int dx, int dy, int rotation);
void place_block();
void clear_lines();
void rotate_block();
void game_over();

// 게임 초기화
void init_game() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);            // 커서 숨김
    keypad(stdscr, TRUE);   // 키 입력 활성화
    timeout(50);            // 약간의 딜레이 추가로 부드러운 입력 처리
    start_color();          // 컬러 사용
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    init_pair(5, COLOR_RED, COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);
    srand(time(NULL));      // 랜덤 초기화

    generate_new_block();   // 첫 블록 생성
    next_block.shape = rand() % 7;
    next_block.rotation = 0;
}

// 필드와 블록 그리기
void draw_field() {
    clear(); // 화면 전체 지우기
    // 필드 그리기
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (field[y][x]) {
                attron(COLOR_PAIR(field[y][x]));
                mvprintw(y + 1, x * 2 + 1, "  ");
                attroff(COLOR_PAIR(field[y][x]));
            }
        }
    }
    // 현재 블록 그리기
    draw_block(current_block);
    mvprintw(0, 0, "Score: %d", score);
    refresh(); // 화면 갱신
}

// 블록 그리기 함수
void draw_block(Block block) {
    int (*shape)[2] = TETROMINOS[block.shape][block.rotation];
    for (int i = 0; i < TETRIS; i++) {
        int x = block.x + shape[i][1];
        int y = block.y + shape[i][0];
        if (y >= 0) { // 화면에 보이는 부분만 그리기
            attron(COLOR_PAIR(block.shape + 1));
            mvprintw(y + 1, x * 2 + 1, "  ");
            attroff(COLOR_PAIR(block.shape + 1));
        }
    }
}

// 새로운 블록 생성
void generate_new_block() {
    current_block = next_block;
    current_block.x = WIDTH / 2 - 2;
    current_block.y = 0;
    next_block.shape = rand() % 7;
    next_block.rotation = 0;

    // 블록이 생성될 위치에 충돌이 발생하면 게임 오버
    if (check_collision(current_block, 0, 0, current_block.rotation)) {
        game_over();
    }
}

// 충돌 검사
bool check_collision(Block block, int dx, int dy, int rotation) {
    int (*shape)[2] = TETROMINOS[block.shape][rotation];
    for (int i = 0; i < TETRIS; i++) {
        int x = block.x + shape[i][1] + dx;
        int y = block.y + shape[i][0] + dy;
        // 필드 밖으로 나가거나 다른 블록과 겹치면 true 반환
        if (x < 0 || x >= WIDTH || y >= HEIGHT || (y >= 0 && field[y][x])) {
            return true;
        }
    }
    return false;
}

// 블록을 필드에 고정
void place_block() {
    int (*shape)[2] = TETROMINOS[current_block.shape][current_block.rotation];
    for (int i = 0; i < TETRIS; i++) {
        int x = current_block.x + shape[i][1];
        int y = current_block.y + shape[i][0];
        if (y >= 0) {
            field[y][x] = current_block.shape + 1;
        }
    }
    clear_lines();
    generate_new_block();
}

// 줄 삭제
void clear_lines() {
    for (int y = HEIGHT - 1; y >= 0; y--) {
        bool full_line = true;
        for (int x = 0; x < WIDTH; x++) {
            if (!field[y][x]) {
                full_line = false;
                break;
            }
        }
        if (full_line) {
            // 한 줄 아래로 당기기
            for (int ty = y; ty > 0; ty--) {
                for (int tx = 0; tx < WIDTH; tx++) {
                    field[ty][tx] = field[ty - 1][tx];
                }
            }
            for (int tx = 0; tx < WIDTH; tx++) {
                field[0][tx] = 0;
            }
            score += 100; // 점수 증가
            y++; // 같은 줄을 다시 검사
        }
    }
}

// 블록 회전
void rotate_block() {
    int new_rotation = (current_block.rotation + 1) % 4;
    if (!check_collision(current_block, 0, 0, new_rotation)) {
        current_block.rotation = new_rotation;
    }
}

// 게임 오버 처리
void game_over() {
    endwin();
    printf("Game Over! Your Score: %d\n", score);
    exit(0);
}

// 메인 함수
int main() {
    init_game();

    while (true) {
        draw_field();

        int ch = getch();
        // 키 입력에 따른 블록 이동
        if (ch == KEY_LEFT && !check_collision(current_block, -1, 0, current_block.rotation)) {
            current_block.x--;
        } else if (ch == KEY_RIGHT && !check_collision(current_block, 1, 0, current_block.rotation)) {
            current_block.x++;
        } else if (ch == KEY_DOWN && !check_collision(current_block, 0, 1, current_block.rotation)) {
            current_block.y++;
        } else if (ch == ' ') { // 스페이스바로 회전
            rotate_block();
        }

        // 자동 하강
        static int counter = 0;
        if (++counter >= 10) { // 속도 조절
            counter = 0;
            if (!check_collision(current_block, 0, 1, current_block.rotation)) {
                current_block.y++;
            } else {
                place_block();
            }
        }
    }

    endwin();
    return 0;
}
