/**
 * @file main.cpp
 * @brief Trivia Game - Player Application (App 1)
 *
 * Manages the player experience: playing the quiz, saving progress,
 * and viewing the leaderboard. All input is via command-line arguments.
 *
 * Usage:
 *   ./main.exe play <player_name>
 *   ./main.exe leaderboard
 *   ./main.exe questions
 *   ./main.exe progress
 *   ./main.exe reset_progress <player_name>
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

// ─────────────────────────────────────────────
// CLASS: Question
// Stores a single trivia question with options,
// correct answer index, and difficulty level.
// ─────────────────────────────────────────────
class Question {
public:
    std::string text;
    std::string options[4];
    int correct;      // 1-4
    int difficulty;   // 1=Easy, 2=Medium, 3=Hard

    Question() : correct(1), difficulty(1) {}

    Question(const std::string& t, const std::string opts[4], int c, int d)
        : text(t), correct(c), difficulty(d) {
        for (int i = 0; i < 4; i++) options[i] = opts[i];
    }

    /**
     * @brief Returns difficulty as a human-readable string.
     */
    std::string difficultyLabel() const {
        if (difficulty == 1) return "Easy";
        if (difficulty == 2) return "Medium";
        return "Hard";
    }

    /**
     * @brief Operator<< overload for displaying a Question to any stream.
     *        Satisfies the operator overloading requirement.
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
// CLASS: Player (base class)
// Holds the player's name and score.
// ─────────────────────────────────────────────
class Player {
protected:
    std::string name;
    int score;

public:
    Player(const std::string& n = "Unknown", int s = 0)
        : name(n), score(s) {}

    virtual ~Player() {}

    std::string getName() const { return name; }
    int getScore()        const { return score; }
    void setScore(int s)        { score = s; }
    void incrementScore()       { score++; }

    /**
     * @brief Saves final score to leaderboard.txt (append mode).
     */
    virtual void saveToLeaderboard() const {
        std::ofstream file("leaderboard.txt", std::ios::app);
        if (file.is_open()) {
            file << name << " " << score << "\n";
        }
    }

    /**
     * @brief Operator<< overload for displaying a Player.
     */
    friend std::ostream& operator<<(std::ostream& os, const Player& p) {
        os << p.name << " - Score: " << p.score;
        return os;
    }
};

// ─────────────────────────────────────────────
// CLASS: GamePlayer (inherits Player)
// Adds progress save/load functionality.
// Association: GamePlayer HAS-A vector<Question>.
// Inheritance: GamePlayer IS-A Player.
// ─────────────────────────────────────────────
class GamePlayer : public Player {
private:
    int questionIndex; // current question position for save/resume

public:
    GamePlayer(const std::string& n, int s = 0, int idx = 0)
        : Player(n, s), questionIndex(idx) {}

    int getQuestionIndex() const { return questionIndex; }
    void setQuestionIndex(int i) { questionIndex = i; }

    /**
     * @brief Saves mid-game progress to progress.txt.
     */
    void saveProgress() const {
        std::ofstream file("progress.txt");
        if (file.is_open())
            file << name << " " << questionIndex << " " << score;
    }

    /**
     * @brief Attempts to load saved progress for this player.
     * @return true if valid saved progress was found.
     */
    bool loadProgress() {
        std::ifstream file("progress.txt");
        if (!file.is_open()) return false;

        std::string savedName;
        int savedIdx, savedScore;
        if (file >> savedName >> savedIdx >> savedScore) {
            if (savedName == name && savedIdx > 0) {
                questionIndex = savedIdx;
                score = savedScore;
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Clears progress file after finishing the game.
     */
    static void clearProgress() {
        std::ofstream file("progress.txt");
        if (file.is_open()) file << "none 0 0";
    }
};

// ─────────────────────────────────────────────
// TEMPLATE FUNCTION: displayList<T>
// Prints any vector of objects that support operator<<.
// Satisfies the template function requirement.
// ─────────────────────────────────────────────
template <typename T>
void displayList(const std::vector<T>& items, const std::string& header) {
    std::cout << "\n==== " << header << " ====\n";
    if (items.empty()) {
        std::cout << "(none)\n";
        return;
    }
    for (size_t i = 0; i < items.size(); i++) {
        std::cout << "\n[" << (i + 1) << "] " << items[i];
    }
    std::cout << "\n";
}

// ─────────────────────────────────────────────
// FUNCTION: loadQuestions
// Reads questions.txt and returns a vector of Question objects.
// ─────────────────────────────────────────────
std::vector<Question> loadQuestions() {
    std::vector<Question> questions;
    std::ifstream file("questions.txt");
    if (!file.is_open()) {
        std::cerr << "Error: could not open questions.txt\n";
        return questions;
    }

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

// ─────────────────────────────────────────────
// COMMAND: play <player_name>
// Runs the quiz for the given player, with progress resume support.
// ─────────────────────────────────────────────
void cmdPlay(const std::string& playerName) {
    std::vector<Question> questions = loadQuestions();
    if (questions.empty()) {
        std::cout << "No questions available. Ask the admin to add some.\n";
        return;
    }

    GamePlayer player(playerName);

    // Check for saved progress
    GamePlayer temp(playerName);
    if (temp.loadProgress()) {
        std::cout << "Saved progress found: question "
                  << (temp.getQuestionIndex() + 1)
                  << ", score " << temp.getScore() << ".\n";
        std::cout << "Resuming from saved position.\n";
        player = temp;
    }

    int startIdx = player.getQuestionIndex();
    int correct = player.getScore();

    for (int i = startIdx; i < (int)questions.size(); i++) {
        std::cout << "\n--- Question " << (i + 1) << " / " << questions.size()
                  << " " << questions[i];
        std::cout << "Your answer (1-4) or 0 to SAVE and exit: ";

        int answer;
        std::cin >> answer;

        if (answer == 0) {
            player.setQuestionIndex(i);
            player.setScore(correct);
            player.saveProgress();
            std::cout << "Progress saved at question " << (i + 1)
                      << " with score " << correct << ".\n";
            return;
        }

        if (answer < 1 || answer > 4) {
            std::cout << "Invalid choice. Try again.\n";
            i--;
            continue;
        }

        if (answer == questions[i].correct) {
            std::cout << "Correct!\n";
            correct++;
        } else {
            std::cout << "Wrong. Correct answer was " << questions[i].correct << ".\n";
        }
    }

    // Game finished
    player.setScore(correct);
    std::cout << "\n=== Game Over! Final score: " << correct
              << " / " << questions.size() << " ===\n";
    player.saveToLeaderboard();
    GamePlayer::clearProgress();
}

// ─────────────────────────────────────────────
// COMMAND: leaderboard
// Displays all entries from leaderboard.txt.
// ─────────────────────────────────────────────
void cmdLeaderboard() {
    std::ifstream file("leaderboard.txt");
    if (!file.is_open()) {
        std::cout << "Leaderboard is empty.\n";
        return;
    }

    std::vector<Player> entries;
    std::string name;
    int score;
    while (file >> name >> score)
        entries.emplace_back(name, score);
    file.close();

    // Sort descending by score
    std::sort(entries.begin(), entries.end(), [](const Player& a, const Player& b) {
        return a.getScore() > b.getScore();
    });

    displayList(entries, "LEADERBOARD");
}

// ─────────────────────────────────────────────
// COMMAND: questions
// Displays all questions from questions.txt.
// ─────────────────────────────────────────────
void cmdQuestions() {
    std::vector<Question> questions = loadQuestions();
    displayList(questions, "ALL QUESTIONS");
}

// ─────────────────────────────────────────────
// COMMAND: progress
// Shows the current saved progress entry.
// ─────────────────────────────────────────────
void cmdProgress() {
    std::ifstream file("progress.txt");
    if (!file.is_open()) {
        std::cout << "No progress file found.\n";
        return;
    }

    std::string name;
    int idx, score;
    if (file >> name >> idx >> score && name != "none") {
        std::cout << "Saved progress: player=" << name
                  << ", next question=" << (idx + 1)
                  << ", score=" << score << "\n";
    } else {
        std::cout << "No active saved progress.\n";
    }
}

// ─────────────────────────────────────────────
// COMMAND: reset_progress <player_name>
// Clears progress.txt for the named player.
// ─────────────────────────────────────────────
void cmdResetProgress(const std::string& playerName) {
    std::ifstream check("progress.txt");
    std::string savedName;
    int idx, score;
    if (check >> savedName >> idx >> score && savedName == playerName) {
        check.close();
        std::ofstream out("progress.txt");
        out << "none 0 0";
        std::cout << "Progress for '" << playerName << "' has been reset.\n";
    } else {
        std::cout << "No saved progress found for '" << playerName << "'.\n";
    }
}

// ─────────────────────────────────────────────
// MAIN — parses command-line arguments (argv)
// No std::cin used for navigation. All commands
// come through argv as required by the teacher.
// ─────────────────────────────────────────────
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Trivia Game - Player App\n\n";
        std::cout << "Usage:\n";
        std::cout << "  ./main play <name>            Start or resume game\n";
        std::cout << "  ./main leaderboard            View leaderboard\n";
        std::cout << "  ./main questions              View all questions\n";
        std::cout << "  ./main progress               View saved progress\n";
        std::cout << "  ./main reset_progress <name>  Reset saved progress\n";
        return 0;
    }

    std::string command = argv[1];

    if (command == "play") {
        if (argc < 3) {
            std::cerr << "Usage: ./main play <player_name>\n";
            return 1;
        }
        cmdPlay(argv[2]);

    } else if (command == "leaderboard") {
        cmdLeaderboard();

    } else if (command == "questions") {
        cmdQuestions();

    } else if (command == "progress") {
        cmdProgress();

    } else if (command == "reset_progress") {
        if (argc < 3) {
            std::cerr << "Usage: ./main reset_progress <player_name>\n";
            return 1;
        }
        cmdResetProgress(argv[2]);

    } else {
        std::cerr << "Unknown command: " << command << "\n";
        std::cerr << "Run './main' with no arguments to see usage.\n";
        return 1;
    }

    return 0;
}
