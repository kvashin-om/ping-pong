#include <ncurses.h>
#include <stdio.h>

#define WIDTH 80       // Ширина
#define HEIGHT 25      // Высота
#define RACKET_SIZE 3  // Размер ракетки
#define WIN_SCORE 21   // Счет
#define FPS 33   // 1000/30 = 16.67 → 17 мс, 30 FPS

int handle_input(void);
int move_racket(int position, int direction);

void frame(int racket_left, int racket_right, int ball_x, int ball_y, int score_player_1,
           int score_player_2) {
    printf("\033[H"); // Перевод в лев. верх. угол терминала
    mvprintw(HEIGHT + 3, 0, "                       PLAYER 1: %2d  |  PLAYER 2: %2d\n", score_player_1, score_player_2);
    for (int x = 0; x < HEIGHT; x++) {
        for (int y = 0; y < WIDTH; y++) {
            char output = ' ';
            if (y == 0 || y == WIDTH - 1)
                output = '|';
            else if (x == 0 || x == HEIGHT - 1)
                output = '-';
            else if (x == ball_x && y == ball_y)
                output = 'o';
            else if (x >= racket_left && x < racket_left + RACKET_SIZE && y == 1)
                output = ']';
            else if (x >= racket_right && x < racket_right + RACKET_SIZE && y == WIDTH - 2)
                output = '[';
            mvaddch(x, y, output);
        }
    }
}

int game(int vector_x, int vector_y, int ball_x, int ball_y, int racket_left, int racket_right) {
    int score_player_1 = 0;
    int score_player_2 = 0;
    frame(racket_left, racket_right, ball_x, ball_y, score_player_1, score_player_2);
    while (score_player_1 < WIN_SCORE && score_player_2 < WIN_SCORE) {
        int input = handle_input();
        if (input == 1)
            racket_left = move_racket(racket_left, -1);
        else if (input == 2)
            racket_left = move_racket(racket_left, 1);
        else if (input == 3)
            racket_right = move_racket(racket_right, -1);
        else if (input == 4)
            racket_right = move_racket(racket_right, 1);

        int target_ball_x = ball_x + vector_x;
        int target_ball_y = ball_y + vector_y;
        if (target_ball_x == 0 || target_ball_x == HEIGHT) {
            vector_x *= -1;
            target_ball_x = ball_x + vector_x;
        }
        if ((target_ball_x >= racket_left && target_ball_x < racket_left + RACKET_SIZE &&
             target_ball_y == 1) ||
            (target_ball_x >= racket_right && target_ball_x < racket_right + RACKET_SIZE &&
             target_ball_y == WIDTH)) {
            vector_y *= -1;
            target_ball_y = ball_y + vector_y;
        }
        if (target_ball_y == 0 || target_ball_y == WIDTH) {
            score_player_1 += target_ball_y == WIDTH;
            score_player_2 += target_ball_y == 0;
            vector_y *= -1;
            target_ball_x = HEIGHT / 2;
            target_ball_y = WIDTH / 2;
        }
        ball_x = target_ball_x;
        ball_y = target_ball_y;

        frame(racket_left, racket_right, ball_x, ball_y, score_player_1, score_player_2);
        napms(FPS);
    }

    return score_player_1 == WIN_SCORE;
}

int move_racket(int position, int direction) {
    int move = position + direction;
    if (move < 1) {
        move = 1;
    } else if (move + RACKET_SIZE > HEIGHT) {
        move = HEIGHT - RACKET_SIZE;
    }
    return move;
}

int handle_input(void) {
    int action = 0;
    int c;

    while ((c = getch()) != ERR) {
        if (c == 'a' || c == 'A')      action = 1;
        else if (c == 'z' || c == 'Z') action = 2;
        else if (c == 'k' || c == 'K') action = 3;
        else if (c == 'm' || c == 'M') action = 4;
    }
    return action;
}

int main(void) {
    /*инициализируем функции библиотеки ncureses*/
    initscr();
    cbreak();
    nodelay(stdscr, TRUE);
    noecho();

    int vector_x = 1;
    int vector_y = 1;
    int ball_x = HEIGHT / 2;
    int ball_y = WIDTH / 2;
    int racket_left = HEIGHT / 2;
    int racket_right = HEIGHT / 2;

    int game_res = game(vector_x, vector_y, ball_x, ball_y, racket_left, racket_right);
    endwin(); // завершение игры

    printf("\033[2J\033[H"); // очистка экрана
    printf("\n\n\n\n\n");
    printf("=========================================\n");
    printf("||                                     ||\n");
    if (game_res == 1) {
        printf("||          🏆 PLAYER 1 WINS! 🏆       ||\n");
    } else {
        printf("||          🏆 PLAYER 2 WINS! 🏆       ||\n");
    }
    printf("||                                     ||\n");
    printf("=========================================\n");
    printf("\n\n\n\n\n");

    return 0;
}