#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

class Question {
public:
    string text;
    string options[4];
    int correct;
    int difficulty;

    Question() : correct(1), difficulty(1) {}

    string difficultyLabel() const {
        if (difficulty == 1) return "Easy";
        if (difficulty == 2) return "Medium";
        return "Hard";
    }

    friend ostream& operator<<(ostream& os, const Question& q) {
        os << "\n================================================\n";
        os << "Difficulty: " << q.difficultyLabel() << "\n";
        os << "================================================\n\n";

        os << q.text << "\n\n";

        for (int i = 0; i < 4; i++) {
            os << i + 1 << ". " << q.options[i] << "\n";
        }

        os << "\nCorrect Answer: " << q.correct << "\n";

        return os;
    }
};

class User {
protected:
    string name;

public:
    User(const string& n = "User") : name(n) {}
    virtual ~User() {}

    virtual string getRole() const {
        return "User";
    }
};

class Admin : public User {
public:
    Admin(const string& n = "Admin") : User(n) {}

    string getRole() const override {
        return "Admin";
    }

    bool authenticate(const string& password) const {
        return password == "admin123";
    }
};

template <typename T>
void displayList(const vector<T>& items, const string& header) {
    cout << "\n================================================\n";
    cout << header << "\n";
    cout << "================================================\n";

    if (items.empty()) {
        cout << "(empty)\n";
        return;
    }

    for (size_t i = 0; i < items.size(); i++) {
        cout << "\n[" << i + 1 << "] " << items[i] << "\n";
    }
}

vector<Question> loadQuestions() {
    vector<Question> questions;

    ifstream file("questions.txt");

    Question q;

    while (getline(file, q.text)) {
        if (q.text.empty()) continue;

        getline(file, q.options[0]);
        getline(file, q.options[1]);
        getline(file, q.options[2]);
        getline(file, q.options[3]);

        file >> q.correct >> q.difficulty;
        file.ignore();

        questions.push_back(q);
    }

    return questions;
}

void saveQuestions(const vector<Question>& questions) {
    ofstream file("questions.txt");

    for (const auto& q : questions) {
        file << q.text << "\n";
        file << q.options[0] << "\n";
        file << q.options[1] << "\n";
        file << q.options[2] << "\n";
        file << q.options[3] << "\n";
        file << q.correct << "\n";
        file << q.difficulty << "\n";
    }
}

void clearScreen() {
    system("cls");
}

void pauseScreen() {
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
}

void showBanner() {
    cout << "================================================\n";
    cout << "                ADMIN PANEL                     \n";
    cout << "================================================\n\n";
}

void viewQuestions() {
    vector<Question> questions = loadQuestions();
    displayList(questions, "QUESTION BANK");
    pauseScreen();
}

void addQuestion() {
    cin.ignore();

    Question q;

    cout << "Question Text: ";
    getline(cin, q.text);

    for (int i = 0; i < 4; i++) {
        cout << "Option " << i + 1 << ": ";
        getline(cin, q.options[i]);
    }

    cout << "Correct Answer (1-4): ";
    cin >> q.correct;

    cout << "Difficulty (1=Easy, 2=Medium, 3=Hard): ";
    cin >> q.difficulty;

    ofstream file("questions.txt", ios::app);

    file << q.text << "\n";
    file << q.options[0] << "\n";
    file << q.options[1] << "\n";
    file << q.options[2] << "\n";
    file << q.options[3] << "\n";
    file << q.correct << "\n";
    file << q.difficulty << "\n";

    cout << "\nQuestion Added Successfully.\n";

    pauseScreen();
}

void deleteQuestion() {
    vector<Question> questions = loadQuestions();

    displayList(questions, "QUESTION BANK");

    int number;

    cout << "\nEnter Question Number To Delete: ";
    cin >> number;

    if (number < 1 || number > questions.size()) {
        cout << "Invalid Question Number.\n";
        pauseScreen();
        return;
    }

    questions.erase(questions.begin() + (number - 1));

    saveQuestions(questions);

    cout << "Question Deleted Successfully.\n";

    pauseScreen();
}

void viewLeaderboard() {
    ifstream file("leaderboard.txt");

    cout << "\n================================================\n";
    cout << "LEADERBOARD\n";
    cout << "================================================\n\n";

    string line;

    while (getline(file, line)) {
        cout << line << "\n";
    }

    pauseScreen();
}

void resetProgress() {
    ofstream file("progress.txt");
    file << "none 0 0";

    cout << "\nProgress Reset Successfully.\n";

    pauseScreen();
}

int main() {
    Admin admin;

    clearScreen();

    cout << "================================================\n";
    cout << "                 ADMIN LOGIN                    \n";
    cout << "================================================\n\n";

    string password;

    cout << "Enter Password: ";
    cin >> password;

    if (!admin.authenticate(password)) {
        cout << "\nAccess Denied.\n";
        return 0;
    }

    while (true) {
        clearScreen();
        showBanner();

        cout << "1. View Questions\n";
        cout << "2. Add Question\n";
        cout << "3. Delete Question\n";
        cout << "4. View Leaderboard\n";
        cout << "5. Reset Progress\n";
        cout << "6. Exit\n\n";

        cout << "Choose Option: ";

        int choice;
        cin >> choice;

        if (choice == 1) {
            clearScreen();
            viewQuestions();

        } else if (choice == 2) {
            clearScreen();
            showBanner();
            addQuestion();

        } else if (choice == 3) {
            clearScreen();
            deleteQuestion();

        } else if (choice == 4) {
            clearScreen();
            viewLeaderboard();

        } else if (choice == 5) {
            clearScreen();
            resetProgress();

        } else if (choice == 6) {
            cout << "\nGoodbye!\n";
            break;

        } else {
            cout << "Invalid Option.\n";
            pauseScreen();
        }
    }

    return 0;
}
