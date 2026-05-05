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

// READ QUESTIONS (just display file)
void readQuestions() {
    FILE *file = fopen("questions.txt", "r");

    if (file == NULL) {
        printf("Error opening questions file!\n");
        return;
    }

    char line[256];

    printf("==== QUESTIONS ====\n\n");

    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }

    fclose(file);

    printf("\nPress ENTER...");
    getchar(); getchar();
}

// SAVE SCORE
void saveScore(int score) {
    FILE *file = fopen("leaderboard.txt", "a");

    if (file == NULL) {
        printf("Error opening leaderboard file!\n");
        return;
    }

    char name[50];

    printf("Enter your name: ");
    scanf("%s", name);

    fprintf(file, "%s %d\n", name, score);

    fclose(file);
}

// SHOW LEADERBOARD
void showLeaderboard() {
    FILE *file = fopen("leaderboard.txt", "r");

    if (file == NULL) {
        printf("No leaderboard found!\n");
        return;
    }

    char line[100];

    printf("==== LEADERBOARD ====\n\n");

    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }

    fclose(file);

    printf("\nPress ENTER...");
    getchar(); getchar();
}

// START GAME (FULL VERSION USING STRUCT)
void startGame() {
    char name[50];

    printf("Enter your name: ");
    scanf("%s", name);

    FILE *file = fopen("questions.txt", "r");

    if (file == NULL) {
        printf("Error opening questions file!\n");
        return;
    }

    struct Question q[100];
    int count = 0;

    while (fscanf(file, " %[^\n]\n%[^\n]\n%[^\n]\n%[^\n]\n%[^\n]\n%d\n%d\n",
        q[count].text,
        q[count].a,
        q[count].b,
        q[count].c,
        q[count].d,
        &q[count].correct,
        &q[count].difficulty) == 7) {

        count++;
    }

    fclose(file);

    int score = 0;
    int startIndex = 0;
    int answer;
    char savedName[50];

    // 🔥 LOAD PROGRESS
    FILE *progress = fopen("progress.txt", "r");
    if (progress != NULL) {
        fscanf(progress, "%s %d %d", savedName, &startIndex, &score);
        fclose(progress);

        if (strcmp(name, savedName) == 0 && startIndex > 0) {
            printf("Continue your previous game? (1 = Yes, 0 = No): ");
            int choice;
            scanf("%d", &choice);

            if (choice == 0) {
                startIndex = 0;
                score = 0;
            }
        }
    }

    // GAME LOOP
    for (int i = startIndex; i < count; i++) {
        printf("\nQuestion %d:\n", i + 1);
        printf("%s\n", q[i].text);
        printf("1. %s\n2. %s\n3. %s\n4. %s\n",
               q[i].a, q[i].b, q[i].c, q[i].d);

        printf("Choose (1-4) or 0 to EXIT: ");
        scanf("%d", &answer);

        if (answer == 0) {
            FILE *progress = fopen("progress.txt", "w");
            fprintf(progress, "%s %d %d", name, i, score);
            fclose(progress);

            printf("Game saved for %s!\n", name);
            return;
        }

        if (answer < 1 || answer > 4) {
            printf(RED "Invalid choice!\n" RESET);
            i--;
            continue;
        }

        if (answer == q[i].correct) {
            printf(GREEN "Correct!\n" RESET);
            score++;
        } else {
            printf(RED "Wrong!\n" RESET);
        }
    }

    // FINISH GAME
    printf("\nFinal score: %d/%d\n", score, count);

    // SAVE FINAL SCORE
    FILE *leader = fopen("leaderboard.txt", "a");
    fprintf(leader, "%s %d\n", name, score);
    fclose(leader);

    // CLEAR PROGRESS
    progress = fopen("progress.txt", "w");
    fprintf(progress, "none 0 0");
    fclose(progress);

    printf("\nPress ENTER...");
    getchar(); getchar();
}
// MAIN MENU
int main() {
    int choice;

    while (1) {
        system("cls"); // for Windows (CodeBlocks)

        printf("==== TRIVIA APP ====\n");
        printf("1. Start Game\n");
        printf("2. View Questions (file)\n");
        printf("3. View Leaderboard\n");
        printf("4. Exit\n");

        printf("Choose option (1-4): ");
        scanf("%d", &choice);

        // VALIDATION
        if (choice < 1 || choice > 4) {
            printf(RED "Invalid option!\n" RESET);
            getchar(); getchar();
            continue;
        }

        switch (choice) {
            case 1:
                startGame();
                break;
            case 2:
                readQuestions();
                break;
            case 3:
                showLeaderboard();
                break;
            case 4:
                printf("Goodbye!\n");
                exit(0);
        }
    }

    return 0;
}
