#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>

using namespace std;

const string GREEN = "\033[1;32m";
const string RED   = "\033[1;31m";
const string RESET = "\033[0m";

const string ADMIN_PASSWORD = "admin123";

struct Question {
    string text;
    string options[4];
    int correct;
    int difficulty;
};

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pause() {
    cout << "\nPress ENTER to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// LOGIN
bool login() {
    string password;
    cout << "Enter admin password: ";
    cin >> password;
    cin.ignore();
    if (password == ADMIN_PASSWORD) {
        cout << GREEN << "Access granted!\n" << RESET;
        return true;
    }
    cout << RED << "Wrong password! Access denied.\n" << RESET;
    return false;
}

// VIEW LEADERBOARD
void viewLeaderboard() {
    ifstream file("leaderboard.txt");
    if (!file.is_open()) {
        cout << "No leaderboard found!\n";
        pause();
        return;
    }

    cout << "\n==== LEADERBOARD ====\n\n";
    string line;
    bool empty = true;
    while (getline(file, line)) {
        if (!line.empty()) {
            cout << line << "\n";
            empty = false;
        }
    }
    file.close();

    if (empty) cout << "(empty)\n";
    pause();
}

// DELETE A PLAYER
void deletePlayer() {
    string targetName;
    cout << "Enter player name to delete: ";
    cin >> targetName;
    cin.ignore();

    ifstream file("leaderboard.txt");
    if (!file.is_open()) {
        cout << RED << "Leaderboard file not found!\n" << RESET;
        pause();
        return;
    }

    vector<string> kept;
    string name;
    int score;
    bool found = false;

    while (file >> name >> score) {
        if (name == targetName) {
            found = true;
        } else {
            kept.push_back(name + " " + to_string(score));
        }
    }
    file.close();

    ofstream out("leaderboard.txt");
    for (const string& line : kept)
        out << line << "\n";
    out.close();

    if (found)
        cout << GREEN << "Player '" << targetName << "' removed!\n" << RESET;
    else
        cout << RED << "Player '" << targetName << "' not found.\n" << RESET;

    pause();
}

// CLEAR LEADERBOARD
void clearLeaderboard() {
    string confirm;
    cout << "Are you sure? Type 'yes' to confirm: ";
    cin >> confirm;
    cin.ignore();

    if (confirm != "yes") {
        cout << "Cancelled.\n";
        pause();
        return;
    }

    ofstream file("leaderboard.txt");
    file.close();
    cout << GREEN << "Leaderboard cleared!\n" << RESET;
    pause();
}

// RESET PROGRESS
void resetProgress() {
    ifstream check("progress.txt");
    if (check.is_open()) {
        string name;
        int idx, score;
        if (check >> name >> idx >> score && name != "none") {
            cout << "Current saved progress: Player=" << name
                 << ", Question=" << idx + 1 << ", Score=" << score << "\n";
        } else {
            cout << "No active progress to reset.\n";
        }
        check.close();
    }

    ofstream file("progress.txt");
    if (file.is_open()) {
        file << "none 0 0";
        file.close();
        cout << GREEN << "Progress reset!\n" << RESET;
    }
    pause();
}

// ADD QUESTION
void addQuestion() {
    Question q;
    cout << "Enter question text: ";
    getline(cin, q.text);

    for (int i = 0; i < 4; i++) {
        cout << "Option " << i + 1 << ": ";
        getline(cin, q.options[i]);
    }

    cout << "Correct answer (1-4): ";
    cin >> q.correct;
    if (q.correct < 1 || q.correct > 4) {
        cout << RED << "Invalid! Must be 1-4. Question not saved.\n" << RESET;
        cin.ignore();
        pause();
        return;
    }

    cout << "Difficulty (1=Easy, 2=Medium, 3=Hard): ";
    cin >> q.difficulty;
    if (q.difficulty < 1 || q.difficulty > 3) {
        cout << RED << "Invalid! Must be 1-3. Question not saved.\n" << RESET;
        cin.ignore();
        pause();
        return;
    }
    cin.ignore();

    ofstream file("questions.txt", ios::app);
    if (!file.is_open()) {
        cout << RED << "Error opening questions.txt!\n" << RESET;
        pause();
        return;
    }

    file << q.text << "\n"
         << q.options[0] << "\n"
         << q.options[1] << "\n"
         << q.options[2] << "\n"
         << q.options[3] << "\n"
         << q.correct << "\n"
         << q.difficulty << "\n";
    file.close();

    cout << GREEN << "Question added successfully!\n" << RESET;
    pause();
}

// VIEW QUESTIONS
void viewQuestions() {
    ifstream file("questions.txt");
    if (!file.is_open()) {
        cout << "No questions file found!\n";
        pause();
        return;
    }

    cout << "\n==== ALL QUESTIONS ====\n";
    Question q;
    int count = 0;

    while (getline(file, q.text)) {
        if (q.text.empty()) continue;
        if (!getline(file, q.options[0])) break;
        if (!getline(file, q.options[1])) break;
        if (!getline(file, q.options[2])) break;
        if (!getline(file, q.options[3])) break;
        file >> q.correct >> q.difficulty;
        file.ignore();

        count++;
        cout << "\n[" << count << "] " << q.text
             << " (Difficulty: " << q.difficulty << ")\n";
        for (int i = 0; i < 4; i++)
            cout << "  " << i + 1 << ". " << q.options[i] << "\n";
        cout << "  Correct: " << q.correct << "\n";
    }
    file.close();

    if (count == 0) cout << "No questions found.\n";
    else cout << "\nTotal: " << count << " question(s)\n";

    pause();
}

// MAIN MENU
int main() {
    cout << "==== ADMIN PANEL ====\n";
    if (!login()) {
        pause();
        return 1;
    }

    int choice;
    while (true) {
        clearScreen();
        cout << "==== ADMIN PANEL ====\n";
        cout << "1. View Leaderboard\n";
        cout << "2. Delete Player\n";
        cout << "3. Clear Leaderboard\n";
        cout << "4. Reset Player Progress\n";
        cout << "5. Add Question\n";
        cout << "6. View All Questions\n";
        cout << "7. Exit\n";
        cout << "Choose option (1-7): ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore();

        switch (choice) {
            case 1: viewLeaderboard(); break;
            case 2: deletePlayer();    break;
            case 3: clearLeaderboard(); break;
            case 4: resetProgress();   break;
            case 5: addQuestion();     break;
            case 6: viewQuestions();   break;
            case 7:
                cout << "Goodbye!\n";
                return 0;
            default:
                cout << RED << "Invalid option!\n" << RESET;
                pause();
        }
    }
}
