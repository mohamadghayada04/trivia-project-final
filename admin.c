#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// VIEW LEADERBOARD
void viewLeaderboard() {
    FILE *file = fopen("leaderboard.txt", "r");

    if (file == NULL) {
        printf("No leaderboard found!\n");
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

// CLEAR LEADERBOARD
void clearLeaderboard() {
    FILE *file = fopen("leaderboard.txt", "w");

    if (file == NULL) {
        printf("Error!\n");
        return;
    }

    fclose(file);

    printf("Leaderboard cleared!\n");
}

// DELETE A SPECIFIC PLAYER
void deletePlayer() {
    FILE *file = fopen("leaderboard.txt", "r");
    FILE *temp = fopen("temp.txt", "w");

    if (file == NULL || temp == NULL) {
        printf("Error!\n");
        return;
    }

    char name[50], lineName[50];
    int score;

    printf("Enter player name to delete: ");
    scanf("%s", name);

    while (fscanf(file, "%s %d", lineName, &score) == 2) {
        if (strcmp(name, lineName) != 0) {
            fprintf(temp, "%s %d\n", lineName, score);
        }
    }

    fclose(file);
    fclose(temp);

    remove("leaderboard.txt");
    rename("temp.txt", "leaderboard.txt");

    printf("Player removed!\n");
}

// RESET PROGRESS
void resetProgress() {
    FILE *file = fopen("progress.txt", "w");

    if (file == NULL) {
        printf("Error!\n");
        return;
    }

    fprintf(file, "none 0 0");

    fclose(file);

    printf("Progress reset!\n");
}

// ADD NEW QUESTION
void addQuestion() {
    FILE *file = fopen("questions.txt", "a");

    if (file == NULL) {
        printf("Error!\n");
        return;
    }

    char text[100], a[50], b[50], c[50], d[50];
    int correct, difficulty;

    getchar(); // clear buffer

    printf("Enter question: ");
    fgets(text, sizeof(text), stdin);

    printf("Option A: ");
    fgets(a, sizeof(a), stdin);

    printf("Option B: ");
    fgets(b, sizeof(b), stdin);

    printf("Option C: ");
    fgets(c, sizeof(c), stdin);

    printf("Option D: ");
    fgets(d, sizeof(d), stdin);

    printf("Correct answer (1-4): ");
    scanf("%d", &correct);

    printf("Difficulty: ");
    scanf("%d", &difficulty);

    fprintf(file, "%s%s%s%s%s%d\n%d\n\n",
        text, a, b, c, d, correct, difficulty);

    fclose(file);

    printf("Question added!\n");
}

// MAIN MENU
int main() {
    int choice;

    while (1) {
        system("cls");

        printf("==== ADMIN PANEL ====\n");
        printf("1. View Leaderboard\n");
        printf("2. Delete Player\n");
        printf("3. Clear Leaderboard\n");
        printf("4. Reset Progress\n");
        printf("5. Add Question\n");
        printf("6. Exit\n");

        printf("Choose option (1-6): ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: viewLeaderboard(); break;
            case 2: deletePlayer(); break;
            case 3: clearLeaderboard(); break;
            case 4: resetProgress(); break;
            case 5: addQuestion(); break;
            case 6: exit(0);
            default:
                printf("Invalid option!\n");
                getchar(); getchar();
        }
    }

    return 0;
}
