#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// COLORS
#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
#define RESET "\033[0m"

// STRUCT
struct Question {
    char text[100];
    char a[50];
    char b[50];
    char c[50];
    char d[50];
    int correct;
    int difficulty;
};

// SHOW LEADERBOARD
void showLeaderboard() {
    FILE *file = fopen("leaderboard.txt", "r");
    if (file == NULL) {
        printf("No leaderboard found!\n");
        printf("\nPress ENTER...");
        getchar(); getchar();
        return;
    }

    char line[100];
    printf("\n==== LEADERBOARD ====\n\n");
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);

    printf("\nPress ENTER...");
    getchar(); getchar();
}

// READ QUESTIONS (display raw file)
void readQuestions() {
    FILE *file = fopen("questions.txt", "r");
    if (file == NULL) {
        printf("Error opening questions file!\n");
        printf("\nPress ENTER...");
        getchar(); getchar();
        return;
    }

    char line[256];
    printf("\n==== QUESTIONS ====\n\n");
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);

    printf("\nPress ENTER...");
    getchar(); getchar();
}

// START GAME
void startGame() {
    char name[50];
    printf("Enter your name: ");
    scanf("%49s", name);

    FILE *file = fopen("questions.txt", "r");
    if (file == NULL) {
        printf("Error opening questions file!\n");
        return;
    }

    struct Question q[100];
    int count = 0;

    // Read questions - each field on its own line
    while (count < 100 &&
           fgets(q[count].text, sizeof(q[count].text), file) &&
           fgets(q[count].a,    sizeof(q[count].a),    file) &&
           fgets(q[count].b,    sizeof(q[count].b),    file) &&
           fgets(q[count].c,    sizeof(q[count].c),    file) &&
           fgets(q[count].d,    sizeof(q[count].d),    file) &&
           fscanf(file, "%d\n%d\n", &q[count].correct, &q[count].difficulty) == 2) {

        // Strip newlines from string fields
        q[count].text[strcspn(q[count].text, "\n")] = 0;
        q[count].a[strcspn(q[count].a, "\n")] = 0;
        q[count].b[strcspn(q[count].b, "\n")] = 0;
        q[count].c[strcspn(q[count].c, "\n")] = 0;
        q[count].d[strcspn(q[count].d, "\n")] = 0;

        count++;
    }
    fclose(file);

    if (count == 0) {
        printf("No questions found! Ask the admin to add some.\n");
        printf("\nPress ENTER...");
        getchar(); getchar();
        return;
    }

    int score = 0;
    int startIndex = 0;
    int answer;
    char savedName[50];

    // LOAD PROGRESS
    FILE *progress = fopen("progress.txt", "r");
    if (progress != NULL) {
        if (fscanf(progress, "%49s %d %d", savedName, &startIndex, &score) == 3) {
            fclose(progress);
            if (strcmp(name, savedName) == 0 && startIndex > 0) {
                printf("Previous progress found! Continue from question %d with score %d? (1 = Yes, 0 = No): ", startIndex + 1, score);
                int choice;
                scanf("%d", &choice);
                if (choice == 0) {
                    startIndex = 0;
                    score = 0;
                }
            } else {
                startIndex = 0;
                score = 0;
            }
        } else {
            fclose(progress);
            startIndex = 0;
            score = 0;
        }
    }

    // GAME LOOP
    for (int i = startIndex; i < count; i++) {
        printf("\n--- Question %d / %d (Difficulty: %d) ---\n", i + 1, count, q[i].difficulty);
        printf("%s\n", q[i].text);
        printf("1. %s\n2. %s\n3. %s\n4. %s\n",
               q[i].a, q[i].b, q[i].c, q[i].d);
        printf("Choose (1-4) or 0 to SAVE & EXIT: ");
        scanf("%d", &answer);

        if (answer == 0) {
            FILE *prog = fopen("progress.txt", "w");
            if (prog) {
                fprintf(prog, "%s %d %d", name, i, score);
                fclose(prog);
            }
            printf("Game saved for %s! (Question %d, Score: %d)\n", name, i + 1, score);
            printf("\nPress ENTER...");
            getchar(); getchar();
            return;
        }

        if (answer < 1 || answer > 4) {
            printf(RED "Invalid choice! Try again.\n" RESET);
            i--;
            continue;
        }

        if (answer == q[i].correct) {
            printf(GREEN "Correct!\n" RESET);
            score++;
        } else {
            printf(RED "Wrong! Correct answer was %d.\n" RESET, q[i].correct);
        }
    }

    // FINISH GAME
    printf("\n=============================\n");
    printf("Game Over! Final score: %d/%d\n", score, count);
    printf("=============================\n");

    // SAVE TO LEADERBOARD
    FILE *leader = fopen("leaderboard.txt", "a");
    if (leader) {
        fprintf(leader, "%s %d\n", name, score);
        fclose(leader);
    }

    // CLEAR PROGRESS
    FILE *prog = fopen("progress.txt", "w");
    if (prog) {
        fprintf(prog, "none 0 0");
        fclose(prog);
    }

    printf("\nPress ENTER...");
    getchar(); getchar();
}

// MAIN MENU
int main() {
    int choice;

    while (1) {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif

        printf("==== TRIVIA GAME ====\n");
        printf("1. Start Game\n");
        printf("2. View Questions\n");
        printf("3. View Leaderboard\n");
        printf("4. Exit\n");
        printf("Choose option (1-4): ");

        if (scanf("%d", &choice) != 1) {
            // Clear bad input
            int c; while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        if (choice < 1 || choice > 4) {
            printf(RED "Invalid option!\n" RESET);
            getchar(); getchar();
            continue;
        }

        switch (choice) {
            case 1: startGame();      break;
            case 2: readQuestions();  break;
            case 3: showLeaderboard(); break;
            case 4:
                printf("Goodbye!\n");
                exit(0);
        }
    }

    return 0;
}
