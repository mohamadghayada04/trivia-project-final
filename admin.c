#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
#define RESET "\033[0m"

#define ADMIN_PASSWORD "admin123"

// VIEW LEADERBOARD
void viewLeaderboard() {
    FILE *file = fopen("leaderboard.txt", "r");
    if (file == NULL) {
        printf("No leaderboard found!\n");
        printf("\nPress ENTER...");
        getchar(); getchar();
        return;
    }

    char line[100];
    printf("\n==== LEADERBOARD ====\n\n");
    int count = 0;
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
        count++;
    }
    fclose(file);

    if (count == 0) printf("(empty)\n");

    printf("\nPress ENTER...");
    getchar(); getchar();
}

// CLEAR LEADERBOARD
void clearLeaderboard() {
    char confirm[10];
    printf("Are you sure you want to clear ALL scores? (yes/no): ");
    scanf("%9s", confirm);

    if (strcmp(confirm, "yes") != 0) {
        printf("Cancelled.\n");
        getchar(); getchar();
        return;
    }

    FILE *file = fopen("leaderboard.txt", "w");
    if (file == NULL) {
        printf(RED "Error opening file!\n" RESET);
        return;
    }
    fclose(file);
    printf(GREEN "Leaderboard cleared!\n" RESET);
    getchar(); getchar();
}

// DELETE A SPECIFIC PLAYER
void deletePlayer() {
    FILE *file = fopen("leaderboard.txt", "r");
    FILE *temp = fopen("temp.txt", "w");

    if (file == NULL || temp == NULL) {
        printf(RED "Error opening files!\n" RESET);
        if (file) fclose(file);
        if (temp) fclose(temp);
        return;
    }

    char name[50], lineName[50];
    int score;
    int found = 0;

    printf("Enter player name to delete: ");
    scanf("%49s", name);

    while (fscanf(file, "%49s %d", lineName, &score) == 2) {
        if (strcmp(name, lineName) != 0) {
            fprintf(temp, "%s %d\n", lineName, score);
        } else {
            found = 1;
        }
    }

    fclose(file);
    fclose(temp);

    remove("leaderboard.txt");
    rename("temp.txt", "leaderboard.txt");

    if (found)
        printf(GREEN "Player '%s' removed!\n" RESET, name);
    else
        printf(RED "Player '%s' not found.\n" RESET, name);

    getchar(); getchar();
}

// RESET A PLAYER'S PROGRESS
void resetProgress() {
    FILE *progress = fopen("progress.txt", "r");
    if (progress != NULL) {
        char savedName[50];
        int idx, score;
        if (fscanf(progress, "%49s %d %d", savedName, &idx, &score) == 3 && strcmp(savedName, "none") != 0) {
            printf("Current saved progress: Player=%s, Question=%d, Score=%d\n", savedName, idx + 1, score);
        } else {
            printf("No active progress to reset.\n");
        }
        fclose(progress);
    }

    FILE *file = fopen("progress.txt", "w");
    if (file == NULL) {
        printf(RED "Error!\n" RESET);
        getchar(); getchar();
        return;
    }
    fprintf(file, "none 0 0");
    fclose(file);
    printf(GREEN "Progress reset!\n" RESET);
    getchar(); getchar();
}

// ADD NEW QUESTION
void addQuestion() {
    FILE *file = fopen("questions.txt", "a");
    if (file == NULL) {
        printf(RED "Error opening questions file!\n" RESET);
        getchar(); getchar();
        return;
    }

    char text[100], a[50], b[50], c[50], d[50];
    int correct, difficulty;

    // Clear leftover newline from scanf
    int ch; while ((ch = getchar()) != '\n' && ch != EOF);

    printf("Enter question text: ");
    fgets(text, sizeof(text), stdin);
    text[strcspn(text, "\n")] = 0;

    printf("Option 1: ");
    fgets(a, sizeof(a), stdin);
    a[strcspn(a, "\n")] = 0;

    printf("Option 2: ");
    fgets(b, sizeof(b), stdin);
    b[strcspn(b, "\n")] = 0;

    printf("Option 3: ");
    fgets(c, sizeof(c), stdin);
    c[strcspn(c, "\n")] = 0;

    printf("Option 4: ");
    fgets(d, sizeof(d), stdin);
    d[strcspn(d, "\n")] = 0;

    printf("Correct answer (1-4): ");
    scanf("%d", &correct);
    if (correct < 1 || correct > 4) {
        printf(RED "Invalid answer! Must be 1-4. Question not saved.\n" RESET);
        fclose(file);
        getchar(); getchar();
        return;
    }

    printf("Difficulty (1=Easy, 2=Medium, 3=Hard): ");
    scanf("%d", &difficulty);
    if (difficulty < 1 || difficulty > 3) {
        printf(RED "Invalid difficulty! Must be 1-3. Question not saved.\n" RESET);
        fclose(file);
        getchar(); getchar();
        return;
    }

    // Write in the exact format that main.c reads
    fprintf(file, "%s\n%s\n%s\n%s\n%s\n%d\n%d\n", text, a, b, c, d, correct, difficulty);
    fclose(file);

    printf(GREEN "Question added successfully!\n" RESET);
    getchar(); getchar();
}

// VIEW ALL QUESTIONS
void viewQuestions() {
    FILE *file = fopen("questions.txt", "r");
    if (file == NULL) {
        printf("No questions file found!\n");
        printf("\nPress ENTER...");
        getchar(); getchar();
        return;
    }

    char text[100], a[50], b[50], c[50], d[50];
    int correct, difficulty, count = 0;

    printf("\n==== ALL QUESTIONS ====\n");

    while (fgets(text, sizeof(text), file) &&
           fgets(a, sizeof(a), file) &&
           fgets(b, sizeof(b), file) &&
           fgets(c, sizeof(c), file) &&
           fgets(d, sizeof(d), file) &&
           fscanf(file, "%d\n%d\n", &correct, &difficulty) == 2) {

        text[strcspn(text, "\n")] = 0;
        a[strcspn(a, "\n")] = 0;
        b[strcspn(b, "\n")] = 0;
        c[strcspn(c, "\n")] = 0;
        d[strcspn(d, "\n")] = 0;

        count++;
        printf("\n[%d] %s (Difficulty: %d)\n", count, text, difficulty);
        printf("  1. %s\n  2. %s\n  3. %s\n  4. %s\n", a, b, c, d);
        printf("  Correct: %d\n", correct);
    }

    fclose(file);

    if (count == 0) printf("No questions found.\n");
    else printf("\nTotal: %d question(s)\n", count);

    printf("\nPress ENTER...");
    getchar(); getchar();
}

// LOGIN
int login() {
    char password[50];
    printf("Enter admin password: ");
    scanf("%49s", password);
    if (strcmp(password, ADMIN_PASSWORD) == 0) {
        printf(GREEN "Access granted!\n" RESET);
        return 1;
    }
    printf(RED "Wrong password! Access denied.\n" RESET);
    return 0;
}

// MAIN MENU
int main() {
    printf("==== ADMIN PANEL ====\n");
    if (!login()) {
        getchar(); getchar();
        return 1;
    }

    int choice;
    while (1) {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif

        printf("==== ADMIN PANEL ====\n");
        printf("1. View Leaderboard\n");
        printf("2. Delete Player\n");
        printf("3. Clear Leaderboard\n");
        printf("4. Reset Player Progress\n");
        printf("5. Add Question\n");
        printf("6. View All Questions\n");
        printf("7. Exit\n");
        printf("Choose option (1-7): ");

        if (scanf("%d", &choice) != 1) {
            int c; while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        switch (choice) {
            case 1: viewLeaderboard(); break;
            case 2: deletePlayer();    break;
            case 3: clearLeaderboard(); break;
            case 4: resetProgress();   break;
            case 5: addQuestion();     break;
            case 6: viewQuestions();   break;
            case 7:
                printf("Goodbye!\n");
                exit(0);
            default:
                printf(RED "Invalid option!\n" RESET);
                getchar(); getchar();
        }
    }

    return 0;
}
