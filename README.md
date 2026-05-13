# Trivia Game - C++ Project

## Project Overview

A two-application C++ trivia quiz system where both apps communicate
through shared text files (bidirectional flow).

**App 1 (main)** — Player application  
**App 2 (admin)** — Admin application

---

## Compile

```bash
g++ -std=c++17 -o main main.cpp
g++ -std=c++17 -o admin admin.cpp
```

---

## App 1 Commands (main)

```bash
./main play <player_name>            # Start or resume a quiz
./main leaderboard                   # View leaderboard
./main questions                     # View all questions
./main progress                      # View saved progress
./main reset_progress <player_name>  # Reset saved progress
```

## App 2 Commands (admin)

```bash
./admin view_questions
./admin add_question <text> <opt1> <opt2> <opt3> <opt4> <correct(1-4)> <difficulty(1-3)>
./admin delete_question <number>
./admin view_leaderboard
./admin delete_player <name>
./admin clear_leaderboard
./admin view_progress
./admin reset_progress
```

---

## Shared Files

| File | Written by | Read by |
|------|-----------|---------|
| questions.txt | Admin (App 2) | Main (App 1) |
| leaderboard.txt | Main (App 1) | Admin (App 2) |
| progress.txt | Main (App 1) | Admin (App 2) |

---

## OOP Features

- `Question` class with `operator<<` overload
- `Player` base class with `operator<<` overload
- `GamePlayer` inherits from `Player` (IS-A relationship)
- `User` base class
- `Admin` inherits from `User` (IS-A relationship)
- `template<typename T> void displayList(...)` template function
- STL `std::vector<Question>`, `std::vector<Player>` throughout
- All input via `argv` — no `std::cin` for navigation
