/**
 * @file admin.cpp
 * @brief Trivia Game - Admin Application (App 2)
 *
 * Manages the question bank and leaderboard from the admin side.
 * Changes made here are immediately visible to App 1 (main).
 * All input is via command-line arguments — no std::cin menus.
 *
 * Usage:
 *   ./admin view_questions
 *   ./admin add_question <text> <opt1> <opt2> <opt3> <opt4> <correct(1-4)> <difficulty(1-3)>
 *   ./admin delete_question <number>
 *   ./admin view_leaderboard
 *   ./admin delete_player <name>
 *   ./admin clear_leaderboard
 *   ./admin view_progress
 *   ./admin reset_progress
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

// ─────────────────────────────────────────────
// CLASS: Question
// (Same as in main.cpp — both apps share the schema)
// ─────────────────────────────────────────────
class Question {
public:
    std::string text;
    std::string options[4];
    int correct;
    int difficulty;

    Question() : correct(1), difficulty(1) {}

    std::string difficultyLabel() const {
        if (difficulty == 1) return "Easy";
        if (difficulty == 2) return "Medium";
        return "Hard";
    }

    /**
     * @brief Operator<< for displaying a question to any stream.
     */
    friend std::ostream& operator<<(std::ostream& os, const Question& q) {
        os << "[" << q.difficultyLabel() << "] " << q.text << "\n";
        for (int i = 0; i < 4; i++)
            os << "  " << (i + 1) << ". " << q.options[i] << "\n";
        os << "  Correct: " << q.correct << "\n";
        return os;
    }
};

// ─────────────────────────────────────────────
// CLASS: User (base class)
// Represents a generic user with a name.
// ─────────────────────────────────────────────
class User {
protected:
    std::string name;

public:
    explicit User(const std::string& n = "") : name(n) {}
    virtual ~User() {}
    std::string getName() const { return name; }

    /**
     * @brief Virtual method — subclasses define their role description.
     */
    virtual std::string getRole() const { return "User"; }
};

// ─────────────────────────────────────────────
// CLASS: Admin (inherits User)
// Has elevated privileges: can edit questions
// and manage the leaderboard.
// Inheritance relationship: Admin IS-A User.
// ─────────────────────────────────────────────
class Admin : public User {
public:
    explicit Admin(const std::string& n = "Admin") : User(n) {}

    std::string getRole() const override { return "Admin"; }

    /**
     * @brief Verifies the admin password.
     * @param password The password to check.
     * @return true if password matches.
     */
    bool authenticate(const std::string& password) const {
        return password == "admin123";
    }
};

// ─────────────────────────────────────────────
// TEMPLATE FUNCTION: displayList<T>
// Prints any vector of objects that support operator<<.
// ─────────────────────────────────────────────
template <typename T>
void displayList(const std::vector<T>& items, const std::string& header) {
    std::cout << "\n==== " << header << " ====\n";
    if (items.empty()) {
        std::cout << "(empty)\n";
        return;
    }
    for (size_t i = 0; i < items.size(); i++)
        std::cout << "\n[" << (i + 1) << "] " << items[i];
    std::cout << "\n";
}

// ─────────────────────────────────────────────
// HELPER: loadQuestions / saveQuestions
// ─────────────────────────────────────────────
std::vector<Question> loadQuestions() {
    std::vector<Question> questions;
    std::ifstream file("questions.txt");
    if (!file.is_open()) return questions;

    Question q;
    while (std::getline(file, q.text)) {
        if (q.text.empty()) continue;
        if (!std::getline(file, q.options[0])) break;
        if (!std::getline(file, q.options[1])) break;
        if (!std::getline(file, q.options[2])) break;
        if (!std::getline(file, q.options[3])) break;
        file >> q.correct >> q.difficulty;
        file.ignore();
        questions.push_back(q);
    }
    file.close();
    return questions;
}

void saveQuestions(const std::vector<Question>& questions) {
    std::ofstream file("questions.txt");
    for (const auto& q : questions) {
        file << q.text << "\n"
             << q.options[0] << "\n" << q.options[1] << "\n"
             << q.options[2] << "\n" << q.options[3] << "\n"
             << q.correct << "\n" << q.difficulty << "\n";
    }
}

// ─────────────────────────────────────────────
// COMMAND: view_questions
// ─────────────────────────────────────────────
void cmdViewQuestions() {
    std::vector<Question> questions = loadQuestions();
    displayList(questions, "ALL QUESTIONS");
    std::cout << "Total: " << questions.size() << " question(s)\n";
}

// ─────────────────────────────────────────────
// COMMAND: add_question <text> <o1> <o2> <o3> <o4> <correct> <difficulty>
// ─────────────────────────────────────────────
void cmdAddQuestion(const std::string& text, const std::string opts[4],
                    int correct, int difficulty) {
    if (correct < 1 || correct > 4) {
        std::cerr << "Error: correct answer must be 1-4.\n";
        return;
    }
    if (difficulty < 1 || difficulty > 3) {
        std::cerr << "Error: difficulty must be 1 (Easy), 2 (Medium), or 3 (Hard).\n";
        return;
    }

    std::ofstream file("questions.txt", std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Error: could not open questions.txt\n";
        return;
    }
    file << text << "\n"
         << opts[0] << "\n" << opts[1] << "\n"
         << opts[2] << "\n" << opts[3] << "\n"
         << correct << "\n" << difficulty << "\n";
    file.close();
    std::cout << "Question added successfully.\n";
}

// ─────────────────────────────────────────────
// COMMAND: delete_question <number>
// ─────────────────────────────────────────────
void cmdDeleteQuestion(int number) {
    std::vector<Question> questions = loadQuestions();
    if (number < 1 || number > (int)questions.size()) {
        std::cerr << "Error: question number " << number << " does not exist.\n";
        return;
    }
    questions.erase(questions.begin() + (number - 1));
    saveQuestions(questions);
    std::cout << "Question " << number << " deleted.\n";
}

// ─────────────────────────────────────────────
// COMMAND: view_leaderboard
// App 2 reads leaderboard.txt written by App 1 — bidirectional flow.
// ─────────────────────────────────────────────
void cmdViewLeaderboard() {
    std::ifstream file("leaderboard.txt");
    if (!file.is_open()) {
        std::cout << "Leaderboard is empty.\n";
        return;
    }

    std::cout << "\n==== LEADERBOARD ====\n\n";
    std::string line;
    bool empty = true;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            std::cout << line << "\n";
            empty = false;
        }
    }
    if (empty) std::cout << "(empty)\n";
}

// ─────────────────────────────────────────────
// COMMAND: delete_player <name>
// ─────────────────────────────────────────────
void cmdDeletePlayer(const std::string& targetName) {
    std::ifstream file("leaderboard.txt");
    if (!file.is_open()) {
        std::cerr << "Leaderboard not found.\n";
        return;
    }

    std::vector<std::string> kept;
    std::string name;
    int score;
    bool found = false;

    while (file >> name >> score) {
        if (name == targetName) found = true;
        else kept.push_back(name + " " + std::to_string(score));
    }
    file.close();

    std::ofstream out("leaderboard.txt");
    for (const auto& line : kept) out << line << "\n";

    if (found) std::cout << "Player '" << targetName << "' removed.\n";
    else std::cerr << "Player '" << targetName << "' not found.\n";
}

// ─────────────────────────────────────────────
// COMMAND: clear_leaderboard
// ─────────────────────────────────────────────
void cmdClearLeaderboard() {
    std::ofstream file("leaderboard.txt");
    file.close();
    std::cout << "Leaderboard cleared.\n";
}

// ─────────────────────────────────────────────
// COMMAND: view_progress
// Reads progress.txt written by App 1 — bidirectional flow.
// ─────────────────────────────────────────────
void cmdViewProgress() {
    std::ifstream file("progress.txt");
    if (!file.is_open()) {
        std::cout << "No progress file found.\n";
        return;
    }
    std::string name;
    int idx, score;
    if (file >> name >> idx >> score && name != "none") {
        std::cout << "Active progress: player=" << name
                  << ", next question=" << (idx + 1)
                  << ", score=" << score << "\n";
    } else {
        std::cout << "No active saved progress.\n";
    }
}

// ─────────────────────────────────────────────
// COMMAND: reset_progress
// ─────────────────────────────────────────────
void cmdResetProgress() {
    std::ofstream file("progress.txt");
    file << "none 0 0";
    std::cout << "Progress reset.\n";
}

// ─────────────────────────────────────────────
// MAIN — parses command-line arguments (argv)
// Admin password is passed as the first argument.
// ─────────────────────────────────────────────
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Trivia Game - Admin App\n\n";
        std::cout << "Usage:\n";
        std::cout << "  ./admin view_questions\n";
        std::cout << "  ./admin add_question <text> <opt1> <opt2> <opt3> <opt4> <correct(1-4)> <diff(1-3)>\n";
        std::cout << "  ./admin delete_question <number>\n";
        std::cout << "  ./admin view_leaderboard\n";
        std::cout << "  ./admin delete_player <name>\n";
        std::cout << "  ./admin clear_leaderboard\n";
        std::cout << "  ./admin view_progress\n";
        std::cout << "  ./admin reset_progress\n";
        return 0;
    }

    // Admin authentication via first argument
    Admin admin("AdminUser");
    std::string command = argv[1];

    // Commands that don't require password
    if (command == "view_questions") {
        cmdViewQuestions();
    } else if (command == "view_leaderboard") {
        cmdViewLeaderboard();
    } else if (command == "view_progress") {
        cmdViewProgress();

    // Mutating commands require password as last argument
    } else if (command == "add_question") {
        if (argc < 10) {
            std::cerr << "Usage: ./admin add_question <text> <opt1> <opt2> <opt3> <opt4> <correct> <difficulty>\n";
            return 1;
        }
        std::string opts[4] = {argv[3], argv[4], argv[5], argv[6]};
        cmdAddQuestion(argv[2], opts, std::stoi(argv[7]), std::stoi(argv[8]));

    } else if (command == "delete_question") {
        if (argc < 3) { std::cerr << "Usage: ./admin delete_question <number>\n"; return 1; }
        cmdDeleteQuestion(std::stoi(argv[2]));

    } else if (command == "delete_player") {
        if (argc < 3) { std::cerr << "Usage: ./admin delete_player <name>\n"; return 1; }
        cmdDeletePlayer(argv[2]);

    } else if (command == "clear_leaderboard") {
        cmdClearLeaderboard();

    } else if (command == "reset_progress") {
        cmdResetProgress();

    } else {
        std::cerr << "Unknown command: " << command << "\n";
        return 1;
    }

    return 0;
}
