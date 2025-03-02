#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

// Macros variable (HEIGHT, WIDTH) declare korechi
#define HEIGHT 20
#define WIDTH 60
#define MAX_HIGHSCORES 5 //  maximum number of high scores define korechi

// Pointers  coordinates of snake tail (x-axis, y-axis) store korbar jonne
int *snakeTailX = NULL;
int *snakeTailY = NULL;
int snakeTailLen;

// Score and signal flags
int gameover, key, score;

// Coordinates of snake's head and fruit
int x, y, fruitx, fruity;

const char *highScoreFile = "highscore.txt"; // High score file path
int highScores[MAX_HIGHSCORES] = {0}; // Static array for high scores

// Function declarations
void setup();
void clearScreen();
void draw();
void input();
void logic(int wrapAround);
void trappedInBox(int level);
void viewHighScore();
void showCredits();
void startGame();
void infinityRun(int level);
void loadHighScores();
void saveHighScores();
void freeSnakeTail();
void reallocSnakeTail();
int main();

void setup() {
    gameover = 0;
    x = WIDTH / 2;
    y = HEIGHT / 2;
    fruitx = rand() % WIDTH;
    fruity = rand() % HEIGHT;
    while (fruitx == 0)
        fruitx = rand() % WIDTH;

    while (fruity == 0)
        fruity = rand() % HEIGHT;

    score = 0;
    snakeTailLen = 0;
    reallocSnakeTail();
}

void clearScreen() {
    COORD cursorPosition;
    cursorPosition.X = 0;
    cursorPosition.Y = 0;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
}

// Function to draw the game field, snake, and fruit
void draw() {
    clearScreen();

    for (int i = 0; i < WIDTH + 2; i++)
        printf("-");
    printf("\n");

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j <= WIDTH; j++) {
            if (j == 0 || j == WIDTH)
                printf("#");
            else if (i == y && j == x)
                printf("O");
            else if (i == fruity && j == fruitx)
                printf("*");
            else {
                int prTail = 0;
                for (int k = 0; k < snakeTailLen; k++) {
                    if (snakeTailX[k] == j && snakeTailY[k] == i) {
                        printf("o");
                        prTail = 1;
                    }
                }
                if (!prTail)
                    printf(" ");
            }
        }
        printf("\n");
    }

    for (int i = 0; i < WIDTH + 2; i++)
        printf("-");
    printf("\n");

    printf("score = %d\n", score);
    printf("High score = %d\n", highScores[0]); // Display the highest score
    printf("Press W, A, S, D for movement.\n");
    printf("Press X to quit the game.\n");
}

void input() {
    if (kbhit()) {
        switch (tolower(getch())) {
        case 'a':
            if (key != 2) key = 1;
            break;
        case 'd':
            if (key != 1) key = 2;
            break;
        case 'w':
            if (key != 4) key = 3;
            break;
        case 's':
            if (key != 3) key = 4;
            break;
        case 'x':
            gameover = 1;
            break;
        }
    }
}

void reallocSnakeTail() {
    snakeTailX = (int *)realloc(snakeTailX, snakeTailLen * sizeof(int));
    snakeTailY = (int *)realloc(snakeTailY, snakeTailLen * sizeof(int));
}

void logic(int wrapAround) {
    int prevX = snakeTailX[0];
    int prevY = snakeTailY[0];
    int prev2X, prev2Y;
    snakeTailX[0] = x;
    snakeTailY[0] = y;
    for (int i = 1; i < snakeTailLen; i++) {
        prev2X = snakeTailX[i];
        prev2Y = snakeTailY[i];
        snakeTailX[i] = prevX;
        snakeTailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    switch (key) {
    case 1:
        x--;
        break;
    case 2:
        x++;
        break;
    case 3:
        y--;
        break;
    case 4:
        y++;
        break;
    default:
        break;
    }

    if (wrapAround) {
        // Infinity loop logic: wrap around edges
        if (x < 0) x = WIDTH - 1;
        else if (x >= WIDTH) x = 0;
        if (y < 0) y = HEIGHT - 1;
        else if (y >= HEIGHT) y = 0;
    } else {
        // Trapped in Box logic: end game if hit wall
        if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
            gameover = 1;
            return;
        }
    }

    for (int i = 0; i < snakeTailLen; i++) {
        if (snakeTailX[i] == x && snakeTailY[i] == y)
            gameover = 1;
    }

    if (x == fruitx && y == fruity) {
        fruitx = rand() % WIDTH;
        fruity = rand() % HEIGHT;
        while (fruitx == 0)
            fruitx = rand() % WIDTH;
        while (fruity == 0)
            fruity = rand() % HEIGHT;
        score += 10;
        snakeTailLen++;
        reallocSnakeTail();

        if (score > highScores[0]) {
            highScores[0] = score;
            saveHighScores();
        }
    }
}

void trappedInBox(int level) {
    setup();
    loadHighScores();

    while (!gameover) {
        draw();
        input();
        logic(0); // No wrap around
        Sleep(50);
    }

    printf("\nGame Over! Final Score: %d\n", score);
    printf("Press Enter to exit...\n");
    getchar();
}

void viewHighScore() {
    printf("\n=============================\n");
    printf("       High Scores           \n");
    printf("=============================\n");

    // Sort the highScores array in descending order
    for (int i = 0; i < MAX_HIGHSCORES - 1; i++) {
        for (int j = i + 1; j < MAX_HIGHSCORES; j++) {
            if (highScores[i] < highScores[j]) {
                int temp = highScores[i];
                highScores[i] = highScores[j];
                highScores[j] = temp;
            }
        }
    }

    for (int i = 0; i < MAX_HIGHSCORES; i++) {
        printf("%d. %d\n", i + 1, highScores[i]);
    }
    printf("Press Enter to return...\n");
    getchar();
}

void showCredits() {
    printf("\n=============================\n");
    printf("       Game Credits\n");
    printf("=============================\n");
    printf("Developers:\n");
    printf("1. Dibbo Adhikary\n");
    printf("   Roll: 2307080\n");
    printf("   CSE, KUET\n");
    printf("2. Shrayashee Saha\n");
    printf("   Roll: 2307079\n");
    printf("   CSE, KUET\n");
    printf("3. Safoan Ahmed Sany\n");
    printf("   Roll: 2307063\n");
    printf("   CSE, KUET\n");
    printf("=============================\n");
    printf("Instructed by:\n");
    printf("   Nazirul Hasan ( Shawon )\n");
    printf("   Lecturer, Dept. of CSE, KUET\n\n"); // Extra newline added here
    printf("   Tajmilur Rahman\n");
    printf("   Lecturer, Dept. of CSE, KUET\n");
    printf("=============================\n");
    printf("Press Enter to return...\n");
    getchar();
}


void infinityRun(int level) {
    setup();
    loadHighScores();

    while (!gameover) {
        draw();
        input();
        logic(1); // Wrap around enabled
        Sleep(50);
    }

    printf("\nGame Over! Final Score: %d\n", score);
    printf("Press Enter to exit...\n");
    getchar();
}

void startGame() {
    int choice;
    printf("Choose a game mode:\n");
    printf("1. Trapped in Box\n");
    printf("2. Infinity Run\n");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            trappedInBox(1); // Start Trapped in Box with level 1
            break;
        case 2:
            infinityRun(1); // Start Infinity Run with level 1
            break;
        default:
            printf("Invalid choice! Returning to main menu.\n");
    }
}

void loadHighScores() {
    FILE *file = fopen(highScoreFile, "r");
    if (file) {
        for (int i = 0; i < MAX_HIGHSCORES; i++) {
            fscanf(file, "%d", &highScores[i]);
        }
        fclose(file);
    } else {
        for (int i = 0; i < MAX_HIGHSCORES; i++) {
            highScores[i] = 0;
        }
    }
}

void saveHighScores() {
    FILE *file = fopen(highScoreFile, "w");
    if (file) {
        for (int i = 0; i < MAX_HIGHSCORES; i++) {
            fprintf(file, "%d\n", highScores[i]);
        }
        fclose(file);
    }
}

void freeSnakeTail() {
    free(snakeTailX);
    free(snakeTailY);
}

int main() {
    srand(time(0));
    int choice;


    while (1) {
        printf("\n=============================\n");
        printf("       Cobra Snake Game      \n");
        printf("=============================\n");
        printf("1. Start Game\n");
        printf("2. View High Scores\n");
        printf("3. Game Credits\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                startGame(); // Call the function to start the game

            // Call the function to start the game
                break;
            case 2:
                viewHighScore();
                break;
            case 3:
                showCredits();
                break;
            case 4:
                printf("Exiting... Goodbye!\n");
                return 0;
            default:
                printf("Invalid choice! Please try again.\n");
        }
    }

    return 0;
}