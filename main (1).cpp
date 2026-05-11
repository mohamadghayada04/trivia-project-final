#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>

using namespace std;

// COLORS
const string GREEN = "\033[1;32m";
const string RED   = "\033[1;31m";
const string RESET = "\033[0m";

// STRUCT
struct Question {
    string text;
    string options[4];
    int correct;
    int difficulty;
};

// CLEAR SCREEN
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// PAUSE
void pause() {
    cout << "\nPress ENTER to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// LOAD QUESTIONS
vector<Question> loadQuestions() {
    vector<Question> questions;
    ifstream file("questions.txt");

    if (!file.is_open()) {
        cout << RED << "Error: could not open questions.txt" << RESET << endl;
        return questions;
    }

    Question q;
    while (getline(file, q.text)) {
        if (q.text.empty()) continue;
        if (!getline(file, q.options[0])) break;
        if (!getline(file, q.options[1])) break;
        if (!getline(file, q.options[2])) break;
        if (!getline(file, q.options[3])) break;
        file >> q.correct >> q.difficulty;
        file.ignore();
        questions.push_back(q);
    }

    file.close();
    return questions;
}

// SHOW LEADERBOARD
void showLeaderboard() {
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

// VIEW QUESTIONS
void viewQuestions() {
    vector<Question> questions = loadQuestions();

    if (questions.empty()) {
        cout << "No questions found!\n";
        pause();
        return;
    }

    cout << "\n==== QUESTIONS ====\n";
    for (int i = 0; i < (int)questions.size(); i++) {
        cout << "\n[" << i + 1 << "] " << questions[i].text
             << " (Difficulty: " << questions[i].difficulty << ")\n";
        for (int j = 0; j < 4; j++)
            cout << "  " << j + 1 << ". " << questions[i].options[j] << "\n";
        cout << "  Correct: " << questions[i].correct << "\n";
    }

    pause();
}

// START GAME
void startGame() {
    string name;
    cout << "Enter your name: ";
    cin >> name;
    cin.ignore();

    vector<Question> questions = loadQuestions();

    if (questions.empty()) {
        cout << "No questions available! Ask the admin to add some.\n";
        pause();
        return;
    }

    int score = 0;
    int startIndex = 0;

    // LOAD PROGRESS
    ifstream prog("progress.txt");
    if (prog.is_open()) {
        string savedName;
        int savedIndex, savedScore;
        if (prog >> savedName >> savedIndex >> savedScore) {
            if (savedName == name && savedIndex > 0) {
                cout << "Previous progress found! Continue from question "
                     << savedIndex + 1 << " with score " << savedScore
                     << "? (1 = Yes, 0 = No): ";
                int choice;
                cin >> choice;
                cin.ignore();
                if (choice == 1) {
                    startIndex = savedIndex;
                    score = savedScore;
                }
            }
        }
        prog.close();
    }

    // GAME LOOP
    for (int i = startIndex; i < (int)questions.size(); i++) {
        clearScreen();
        cout << "--- Question " << i + 1 << " / " << questions.size()
             << " (Difficulty: " << questions[i].difficulty << ") ---\n";
        cout << questions[i].text << "\n";
        for (int j = 0; j < 4; j++)
            cout << j + 1 << ". " << questions[i].options[j] << "\n";
        cout << "Choose (1-4) or 0 to SAVE & EXIT: ";

        int answer;
        if (!(cin >> answer)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            i--;
            continue;
        }
        cin.ignore();

        if (answer == 0) {
            ofstream save("progress.txt");
            if (save.is_open()) {
                save << name << " " << i << " " << score;
                save.close();
            }
            cout << "Game saved! (Question " << i + 1 << ", Score: " << score << ")\n";
            pause();
            return;
        }

        if (answer < 1 || answer > 4) {
            cout << RED << "Invalid choice!\n" << RESET;
            i--;
            continue;
        }

        if (answer == questions[i].correct) {
            cout << GREEN << "Correct!\n" << RESET;
            score++;
        } else {
            cout << RED << "Wrong! Correct answer was "
                 << questions[i].correct << ".\n" << RESET;
        }

        cout << "\nPress ENTER...";
        cin.get();
    }

    // GAME OVER
    cout << "\n=============================\n";
    cout << "Game Over! Final score: " << score << "/" << questions.size() << "\n";
    cout << "=============================\n";

    // SAVE TO LEADERBOARD
    ofstream leader("leaderboard.txt", ios::app);
    if (leader.is_open()) {
        leader << name << " " << score << "\n";
        leader.close();
    }

    // CLEAR PROGRESS
    ofstream save("progress.txt");
    if (save.is_open()) {
        save << "none 0 0";
        save.close();
    }

    pause();
}

// MAIN MENU
int main() {
    int choice;

    while (true) {
        clearScreen();
        cout << "==== TRIVIA GAME ====\n";
        cout << "1. Start Game\n";
        cout << "2. View Questions\n";
        cout << "3. View Leaderboard\n";
        cout << "4. Exit\n";
        cout << "Choose option (1-4): ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore();

        switch (choice) {
            case 1: startGame();      break;
            case 2: viewQuestions();  break;
            case 3: showLeaderboard(); break;
            case 4:
                cout << "Goodbye!\n";
                return 0;
            default:
                cout << RED << "Invalid option!\n" << RESET;
                pause();
        }
    }
}
