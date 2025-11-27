#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <set>
#include <map>
#include <algorithm>
#include <sstream>
#include "fenwick.hpp"
#define all(v) std::begin(v), std::end(v)
#define rall(v) std::rbegin(v), std::rend(v)

struct elem { ///name + score + time/penalty(tie-breaker)
    std::string name;
    int pen; ///in minutes

    bool operator<(const elem& o) const {
        if (pen != o.pen) {
            return pen < o.pen;
        }

        return name < o.name;
    }
};

class Leaderboard {
    int n = 0;
    aib fen;
    std::unordered_map<std::string, std::pair<int, int>> mp;
    std::vector<int> v;
    std::unordered_map<int, int> sc_to_p;
    std::map<int, std::set<elem>, std::greater<int>> bckt;

    int score_to_pos(int score) {
        auto it = sc_to_p.find(score);

        if (it == std::end(sc_to_p)) {
            return -1;
        }

        return it->second;
    }

    int pos_to_score(int p) {
        return v[p - 1];
    }

    void rebuild_compression() {
        v.clear();
        v.reserve(sc_to_p.size());

        for (const auto& p : sc_to_p) {
            v.push_back(p.first);
        }

        std::stable_sort(all(v));
        v.erase(std::unique(all(v)), std::end(v));
        n = (int)v.size();
        sc_to_p.clear();

        for (int i = 0; i < n; ++i) {
            sc_to_p[v[i]] = i + 1;
        }

        fen.resize(n);
        for (const auto& e : bckt) {
            int sc = e.first, cnt = (int)e.second.size(), poz = score_to_pos(sc);

            if (poz != -1 && cnt > 0) {
                fen.update(poz, cnt);
            }
        }
    }

    void ensure_score(int score) {
        if (sc_to_p.find(score) != std::end(sc_to_p)) {
            return;
        }

        sc_to_p[score] = 0;
        rebuild_compression();
    }
public:
    Leaderboard() = default;

    explicit Leaderboard(const std::vector<int>& a) {
        v = a;
        std::stable_sort(all(v));
        v.erase(unique(all(v)), std::end(v));
        n = (int)v.size();

        for (int i = 0; i < n; ++i) {
            sc_to_p[v[i]] = i + 1;
        }

        fen.resize(n);
    }

    bool add_contestant(const std::string& name) {
        if (mp.count(name)) {
            return false;
        }

        int score = 0, pen = 0;
        ensure_score(score);
        int p = score_to_pos(score);
        mp[name] = std::make_pair(score, pen);
        fen.update(p, 1);
        bckt[score].insert(elem{name, pen});
        return true;
    }

    bool record_submission(const std::string& name, int sc_delta = 0, int pen_delta = 0) {
        if (!mp.count(name)) {
            add_contestant(name);
        }

        auto curr = mp[name];
        auto [sc, pen] = curr;
        int p = score_to_pos(sc);

        if (p != -1) {
            fen.update(p, -1);
        }

        auto it = bckt.find(sc);

        if (it != std::end(bckt)) {
            it->second.erase(elem{name, pen});

            if (it->second.empty()) {
                bckt.erase(it);
            }
        }

        int nsc = sc + sc_delta, npen = pen + pen_delta;
        ensure_score(nsc);
        int np = score_to_pos(nsc);
        mp[name] = std::make_pair(nsc, npen);
        bckt[nsc].insert(elem{name, npen});
        return true;
    }

    bool set_score_penalty(const std::string name, int score, int penalty) {
        auto it = mp.find(name);

        if (it == std::end(mp)) {
            ensure_score(score);
            int p = score_to_pos(score);
            mp[name] = std::make_pair(score, penalty);
            fen.update(p, 1);
            bckt[score].insert(elem{name, penalty});
            return true;
        }

        auto [sc, pen] = it->second;

        if (score == sc && penalty == pen) {
            return false;
        }

        int p = score_to_pos(sc);

        if (p != -1) {
            fen.update(p, -1);
        }

        auto b_it = bckt.find(sc);

        if (b_it != std::end(bckt)) {
            b_it->second.erase(elem{name, pen});

            if (b_it->second.empty()) {
                bckt.erase(b_it);
            }
        }

        ensure_score(score);
        p = score_to_pos(score);
        mp[name] = std::make_pair(score, penalty);
        fen.update(p, 1);
        bckt[score].insert(elem{name, penalty});
        return true;
    }

    int get_score(const std::string& name) {
        auto it = mp.find(name);

        if (it == std::end(mp)) {
            return -1;
        }

        return it->second.first;
    }

    int get_pen(const std::string& name) {
        auto it = mp.find(name);

        if (it == std::end(mp)) {
            return -1;
        }

        return it->second.second;
    }

    int get_rank(const std::string& name) {
        auto it = mp.find(name);

        if (it == std::end(mp)) {
            return -1;
        }

        auto [sc, pen] = it->second;
        int p = score_to_pos(sc);

        if (p == -1) {
            return -1;
        }

        int nr = fen.range_query(p + 1, n);
        int same_before = 0;
        auto b_it = bckt.find(sc);

        if (b_it != std::end(bckt)) {
            for (const auto& e : b_it->second) {
                if (e.pen < pen) {
                    ++same_before;
                } else {
                    break;
                }
            }
        }

        return nr + same_before + 1;
    }

    int get_kth(int k) { ///kth ranked score
        int tot = fen.query(n);

        if (k < 1 || k > tot) {
            return -1;
        }

        int nev = tot - k + 1;
        int p = fen.kth(nev);
        return pos_to_score(p);
    }

    std::vector<std::tuple<std::string, int, int>> get_top(int k) const {
        std::vector<std::tuple<std::string, int, int>> ans;

        if (k <= 0) {
            return ans;
        }

        for (const auto& e : bckt) {
            int sc = e.first;
            for (const auto& p : e.second) {
                ans.emplace_back(p.name, sc, p.pen);

                if ((int)ans.size() == k) {
                    return ans;
                }
            }
        }

        return ans;
    }

    void print_all(std::ostream& os = std::cout) {
        int cnt = 0;
        for (const auto& e : bckt) {
            int sc = e.first;
            for (const auto& p : e.second) {
                os << ++cnt << ".  " << p.name << " | Score: " << sc << " | Penalty (in minutes): " << p.pen << "\n";
            }
        }
    }

    bool is(const std::string& name) {
        return mp.count(name);
    }

    int dim() {
        return (int)mp.size();
    }
};
