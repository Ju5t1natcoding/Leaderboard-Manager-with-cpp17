#pragma once
#include <vector>
#include <cassert>
#define lb(i) i & -i

struct aib {
    int n = 0;
    std::vector<int> f;

    void resize(int _n) {
        n = _n;
        f.assign(n + 1, 0);
    }

    aib() = default;

    explicit aib(int _n) {
        resize(_n);
    }

    void update(int i, int v) {
        assert(i >= 1 && i <= n);

        for (; i <= n; i += lb(i)) {
            f[i] += v;
        }
    }

    int query(int i) {
        assert(i >= 0 && i <= n);

        int s = 0;
        for (; i; i -= lb(i)) {
            s += f[i];
        }

        return s;
    }

    int range_query(int l, int r) {
        if (l > r) {
            return 0;
        }

        return query(r) - query(l - 1);
    }

    int kth(int k) {
        assert(n > 0);

        int p = 0, pw = 1;

        while ((pw << 1) <= n) pw <<= 1;

        for (int i = pw; i; i >>= 1) {
            int np = p + i;

            if (np <= n && f[np] < k) {
                k -= f[np];
                p = np;
            }
        }

        return p + 1;
    }
};
