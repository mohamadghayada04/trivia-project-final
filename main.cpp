#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

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

        return os;
    }
};

class Player {
protected:
    string name;
    int score;

public:
    Player(const string& n = "Unknown", int s = 0)
        : name(n), score(s) {}

    virtual ~Player() {}

    string getName() const { return name; }
    int getScore() const { return score; }

    void incrementScore() {
        score++;
    }

    virtual void saveToLeaderboard() const {
        ofstream file("leaderboard.txt", ios::app);

        if (file.is_open()) {
            file << name << " " << score << "\n";
        }
    }

    friend ostream& operator<<(ostream& os, const Player& p) {
        os << p.name << " - Score: " << p.score;
        return os;
    }
};

class GamePlayer : public Player {
private:
    int questionIndex;

public:
    GamePlayer(const string& n, int s = 0, int idx = 0)
        : Player(n, s), questionIndex(idx) {}

    int getQuestionIndex() const {
        return questionIndex;
    }

    void setQuestionIndex(int i) {
        questionIndex = i;
    }

    void saveProgress() const {
        ofstream file("progress.txt");

        if (file.is_open()) {
            file << name << " " << questionIndex << " " << score;
        }
    }

    bool loadProgress() {
        ifstream file("progress.txt");

        if (!file.is_open()) return false;

        string savedName;
        int savedIndex;
        int savedScore;

        if (file >> savedName >> savedIndex >> savedScore) {
            if (savedName == name && savedName != "none") {
                questionIndex = savedIndex;
                score = savedScore;
                return true;
            }
        }

        return false;
    }

    static void clearProgress() {
        ofstream file("progress.txt");
        file << "none 0 0";
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

    if (!file.is_open()) {
        return questions;
    }

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
    cout << "              TRIVIA QUIZ SYSTEM                \n";
    cout << "================================================\n\n";
}

void playQuiz(const string& playerName) {
    vector<Question> questions = loadQuestions();

    if (questions.empty()) {
        cout << "No questions found.\n";
        pauseScreen();
        return;
    }

    GamePlayer player(playerName);

    if (player.loadProgress()) {
        cout << "Saved progress loaded successfully.\n";
    }

    int start = player.getQuestionIndex();

    for (int i = start; i < questions.size(); i++) {
        clearScreen();
        showBanner();

        cout << "Question " << i + 1 << " / " << questions.size() << "\n";

        cout << questions[i];

        cout << "\n0. Save and Exit\n\n";
        cout << "Your Answer: ";

        int answer;
        cin >> answer;

        if (answer == 0) {
            player.setQuestionIndex(i);
            player.saveProgress();

            cout << "\nProgress Saved Successfully.\n";
            pauseScreen();
            return;
        }

        if (answer == questions[i].correct) {
            cout << "\nCorrect Answer!\n";
            player.incrementScore();
        } else {
            cout << "\nWrong Answer. Correct answer was: "
                 << questions[i].correct << "\n";
        }

        pauseScreen();
    }

    clearScreen();
    showBanner();

    cout << "QUIZ COMPLETED\n\n";
    cout << "Final Score: " << player.getScore()
         << " / " << questions.size() << "\n";

    player.saveToLeaderboard();
    GamePlayer::clearProgress();

    pauseScreen();
}

void viewLeaderboard() {
    ifstream file("leaderboard.txt");

    vector<Player> players;

    string name;
    int score;

    while (file >> name >> score) {
        players.emplace_back(name, score);
    }

    sort(players.begin(), players.end(),
        [](const Player& a, const Player& b) {
            return a.getScore() > b.getScore();
        });

    displayList(players, "LEADERBOARD");

    pauseScreen();
}

void viewQuestions() {
    vector<Question> questions = loadQuestions();
    displayList(questions, "ALL QUESTIONS");
    pauseScreen();
}

void viewProgress() {
    ifstream file("progress.txt");

    string name;
    int idx;
    int score;

    cout << "\n================================================\n";
    cout << "SAVED PROGRESS\n";
    cout << "================================================\n\n";

    if (file >> name >> idx >> score && name != "none") {
        cout << "Player: " << name << "\n";
        cout << "Next Question: " << idx + 1 << "\n";
        cout << "Score: " << score << "\n";
    } else {
        cout << "No saved progress found.\n";
    }

    pauseScreen();
}

int main() {
    while (true) {
        clearScreen();
        showBanner();

        cout << "1. Play Quiz\n";
        cout << "2. View Leaderboard\n";
        cout << "3. View Questions\n";
        cout << "4. View Progress\n";
        cout << "5. Exit\n\n";

        cout << "Choose Option: ";

        int choice;
        cin >> choice;

        if (choice == 1) {
            clearScreen();
            showBanner();

            string playerName;
            cout << "Enter Player Name: ";
            cin >> playerName;

            playQuiz(playerName);

        } else if (choice == 2) {
            clearScreen();
            viewLeaderboard();

        } else if (choice == 3) {
            clearScreen();
            viewQuestions();

        } else if (choice == 4) {
            clearScreen();
            viewProgress();

        } else if (choice == 5) {
            cout << "\nGoodbye!\n";
            break;

        } else {
            cout << "Invalid Option.\n";
            pauseScreen();
        }
    }

    return 0;
}
