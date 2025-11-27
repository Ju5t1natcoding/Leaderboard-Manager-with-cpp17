#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <chrono>
#include <thread>
#include <windows.h>
#include <conio.h>
#include "leaderboard.hpp"
using namespace std;

static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

void clearScreen() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD written;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    DWORD cells = csbi.dwSize.X * csbi.dwSize.Y;
    COORD home = {0, 0};
    FillConsoleOutputCharacter(hConsole, ' ', cells, home, &written);
    SetConsoleCursorPosition(hConsole, home);
}

void setColor(WORD attributes) {
    SetConsoleTextAttribute(hConsole, attributes);
}

void setCursorPos(int x, int y) {
    COORD coord = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(hConsole, coord);
}

int getKey() {
    int c = _getch();

    if (c == 0 || c == 224) {
        int arrow = _getch();

        switch (arrow) {
            case 72:
                return -1; // up
            case 80:
                return 1;  // down
            case 75:
                return -2; // left
            case 77:
                return 2;  // right
            default:
                return 0;
        }
    }

    return c;
}

string trim(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");

    if (a == string::npos) {
        return "";
    }

    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

void save_to_file(Leaderboard& lb, const string& filename = "saved.txt") {
    ofstream fout(filename);

    if (!fout) {
        return;
    }

    int n = lb.dim();
    auto all = lb.get_top(n);

    for (auto &t : all) {
        auto [name, score, penalty] = t;
        fout << name << "|" << score << "|" << penalty << "\n";
    }

    fout.close();
}

void load_from_file(Leaderboard& lb, const string& filename = "saved.txt") {
    ifstream fin(filename);

    if (!fin) {
        return;
    }

    string line;

    while (getline(fin, line)) {
        line = trim(line);

        if (line.empty()) {
            continue;
        }

        size_t p1 = line.find('|');
        size_t p2 = (p1 == string::npos) ? string::npos : line.find('|', p1 + 1);

        if (p1 == string::npos || p2 == string::npos) {
            continue;
        }

        string name = line.substr(0, p1);
        int score = stoi(line.substr(p1 + 1, p2 - p1 - 1)), penalty = stoi(line.substr(p2 + 1));
        lb.set_score_penalty(name, score, penalty);
    }

    fin.close();
}

void screen_message(const string& title, const string& msg) {
    clearScreen();
    setCursorPos(1, 1);
    setColor(FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    cout << "==== " << title << " ====\n\n";
    setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    cout << msg << "\n\n";
    cout << "Press any key to continue...";
    getKey();
}

void interact_add(Leaderboard& lb) {
    clearScreen();
    setCursorPos(1, 1);
    setColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    cout << "==== ADAUGA CONTESTANT ====\n\n";
    setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    cout << "Enter contestant's name: ";
    string name;
    getline(cin, name);
    name = trim(name);

    if (name.empty()) {
        screen_message("Add", "Invalid name.");
        return;
    }

    if (lb.add_contestant(name)) {
        screen_message("Add", "Competitor added: " + name);
    } else {
        screen_message("Add", "Competitor already exists: " + name);
    }
}

void interact_submit(Leaderboard& lb) {
    clearScreen();
    setCursorPos(1, 1);
    setColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    cout << "==== SUBMIT (record) ====\n\n";
    setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    cout << "Contestant's name: ";
    string name;
    getline(cin, name);
    name = trim(name);

    if (name.empty()) {
        screen_message("Submit", "Invalid name.");
        return;
    }

    cout << "Added score: ";
    string st;
    getline(cin, st);
    int sc = 0;

    try {
        sc = stoi(st);
    } catch(...) {
        sc = 0;
    }

    int pen = 0;
    cout << "Added penalty: ";
    string sp;
    getline(cin, sp);

    try {
        pen = stoi(sp);
    } catch(...) {
        pen = 0;
    }

    lb.record_submission(name, sc, pen);
    screen_message("Submit", "Registerd for " + name);
}

void interact_set(Leaderboard& lb) {
    clearScreen();
    setCursorPos(1, 1);
    setColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    cout << "==== SET SCORE & PENALTY ====\n\n";
    setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    cout << "Contestant's name: ";
    string name;
    getline(cin, name);
    name = trim(name);

    if (name.empty()) {
        screen_message("Set", "Invalid name.");
        return;
    }

    cout << "Score (integer): ";
    string sscore;
    getline(cin, sscore);
    cout << "Penalty (integer): ";
    string spen;
    getline(cin, spen);
    int score = 0, penalty = 0;

    try {
        score = stoi(sscore);
    } catch(...) {
        score = 0;
    }

    try {
        penalty = stoi(spen);
    } catch(...) {
        penalty = 0;
    }

    lb.set_score_penalty(name, score, penalty);
    screen_message("Set", "Values set for " + name);
}

void interact_show(Leaderboard& lb) {
    clearScreen();
    setCursorPos(1, 1);
    setColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    cout << "==== ARATA COMPETITOR ====\n\n";
    setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    cout << "Contestant's name: ";
    string name;
    getline(cin, name);
    name = trim(name);

    if (name.empty()) {
        screen_message("Show", "Invalid name.");
        return;
    }

    if (!lb.is(name)) {
        screen_message("Show", "Competitor does not exist: " + name);
        return;
    }

    int score = lb.get_score(name);
    int penalty = lb.get_pen(name);
    int rank = lb.get_rank(name);

    ostringstream oss;
    oss << "Name: " << name << "\n";
    oss << "Score: " << score << "\n";
    oss << "Penalty: " << penalty << "\n";
    oss << "Rank: " << rank << "\n";
    screen_message("Info Competitor", oss.str());
}

void interact_top(Leaderboard& lb) {
    clearScreen();
    setCursorPos(1, 1);
    setColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    cout << "==== TOP K ====\n\n";
    setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    cout << "K = ";
    string sk;
    getline(cin, sk);
    int k = 0;

    try {
        k = stoi(sk);
    } catch(...) {
        k = 0;
    }

    if (k <= 0) {
        screen_message("Top", "K invalid.");
        return;
    }

    auto top = lb.get_top(k);

    if (top.empty()) {
        screen_message("Top", "Empty leaderboard.");
        return;
    }

    clearScreen();
    setCursorPos(1, 1);
    setColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    cout << "==== TOP " << k << " ====\n\n";
    setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    int cnt = 0;

    for (auto& t : top) {
        auto [name, score, penalty] = t;
        cout << ++cnt << ". " << name << " | score=" << score << " | penalty=" << penalty << "\n";
    }

    cout << "\nPress any key to continue...";
    getKey();
}

void interact_print(Leaderboard& lb) {
    clearScreen();
    setCursorPos(1, 1);
    setColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    cout << "==== FULL LEADERBOARD ====\n\n";
    setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    lb.print_all();
    cout << "\nPress any key to continue...";
    getKey();
}

int main() {
    Leaderboard lb;

    cout << "Do you wish to load from file? (Y / N): ";
    string ans;
    getline(cin, ans);
    clearScreen();

    if (ans == "Y" || ans == "y") load_from_file(lb);

    vector<string> opts = {
        "1. Add contestant",
        "2. Record submission",
        "3. Set score & penalty",
        "4. Show competitor",
        "5. Top K",
        "6. Print all",
        "7. Save and exit"
    };
    int selected = 0;

    while (true) {
        clearScreen();
        setCursorPos(1, 1);
        setColor(FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "==== LEADERBOARD MANAGER ====\n\n";
        setColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        setCursorPos(3, 3);
        cout << "Use up/down arrow keys to navigate and ENTER to select.\n\n";

        for (int i = 0; i < (int)opts.size(); ++i) {
            if (i == selected) {
                setColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
                setCursorPos(1, 6 + i);
                cout << ">";
            } else {
                setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }

            setCursorPos(4, 6 + i);
            cout << opts[i] << "\n";
        }

        int key = getKey();

        if (key == -1 && selected > 0) {
            selected--;
        } else if (key == 1 && selected + 1 < (int)opts.size()) {
            selected++;
        } else if (key == 13) { ///ENTER
            switch (selected) {
                case 0:
                    interact_add(lb);
                    break;
                case 1:
                    interact_submit(lb);
                    break;
                case 2:
                    interact_set(lb);
                    break;
                case 3:
                    interact_show(lb);
                    break;
                case 4:
                    interact_top(lb);
                    break;
                case 5:
                    interact_print(lb);
                    break;
                case 6:
                    setColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                    cout << "Do you wish to save the current leaderboard before exiting? (Y / N): ";
                    setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                    string ans;
                    getline(cin, ans);

                    if (!ans.empty() && (ans[0] == 'Y' || ans[0] == 'y')) {
                        save_to_file(lb);
                    }

            exit(0);
            }
        } else if (key == 27) { // ESC
            clearScreen();
            setCursorPos(1, 1);
            setColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            cout << "Do you wish to save the current leaderboard before exiting? (Y / N): ";
            setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            string ans;
            getline(cin, ans);

            if (!ans.empty() && (ans[0] == 'Y' || ans[0] == 'y')) {
                save_to_file(lb);
            }

            exit(0);
        }
    }
}
