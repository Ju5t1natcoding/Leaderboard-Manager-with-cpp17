# Leaderboard Manager (C++17)

A console-based leaderboard manager written in **C++17**, designed for competitive-programming style scoring (score + penalty tie-breaker).
The project uses a **Fenwick Tree (Binary Indexed Tree)** with coordinate compression for efficient ranking operations.

The interface is a menu-driven console UI (arrow keys + Enter + Esc) created for Windows.
A complete Code::Blocks project is included.

---

## 🎯 Features

* Add contestants with initial score/penalty
* Register submissions:

  * Accepted / Rejected
  * Automatic penalty (e.g., wrong attempts add extra minutes)
* Manually set score or penalty
* Retrieve a contestant’s **rank**
* Get **Top-K** contestants
* Display the full leaderboard ordered by:

  1. Score (descending)
  2. Penalty (ascending)
  3. Name (lexicographically)
* Save/load leaderboard from file (`leaderboard.txt`)
* Efficient data structures:

  * Fenwick Tree for rank queries
  * Ordered buckets for fast traversal

---

## 📂 Project Structure

```
├── main.cpp
├── leaderboard.hpp
├── fenwick.hpp
├── leaderboard.cbp        # Code::Blocks project
├── saved.txt / leaderboard.txt
└── README.md
```

---

## 🔧 Requirements

* **Windows** (uses WinAPI + conio.h for input)
* **C++17 compiler** (MinGW g++ recommended)
* **Code::Blocks 20.03** for easiest compilation

---

## 🚀 Build & Run

### Using Code::Blocks (recommended)

1. Open `leaderboard.cbp`
2. Ensure **-std=c++17** is enabled
3. Build & Run

### Using g++ (MinGW)

```bash
g++ -std=c++17 main.cpp -o leaderboard.exe
./leaderboard.exe
```

> Note: Linux builds require replacing Windows-specific console code.

---

## 💾 File Persistence

The leaderboard is saved as plain text:

```
name|score|penalty
name|score|penalty
...
```

It loads automatically at startup if the file exists.

---

## 🧠 Algorithms Used

* **Fenwick Tree (Binary Indexed Tree)**

  * Rank queries in O(log n)
  * `kth` order-statistic support
* **Coordinate Compression** for score mapping
* **std::map + std::set** for ordered buckets
* Complexity suitable for **OJI/ONI 11–12** level tasks

---

## 🌱 Possible Enhancements

* Full ICPC-style problem tracking
* Export to CSV/JSON
* Cross-platform UI (ncurses)
* Colored output
* Automated testing

---

## 🤝 Contributing

Issues and pull requests are welcome.
If you want others to reuse the project freely, consider adding a LICENSE file (MIT, Apache 2.0, GPLv3, etc.).

---

## 📜 License

This repository currently **does not include a license**.
Without one, the project is fully copyrighted and reuse requires permission.

If you want, I can generate and attach a **MIT License** (most common for such projects).

---

## 📧 Contact

For questions or suggestions, open an Issue or PR on GitHub.

---
