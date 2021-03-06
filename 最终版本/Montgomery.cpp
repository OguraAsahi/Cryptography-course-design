#include <cstdio>
#include <gmpxx.h>
#include <iostream>
#include <utility>

using namespace std;

template <typename T> inline T exgcd(T a, T b, T& x, T& y)
{
    if (b == 0) {
        x = 1, y = 0;
        return a;
    }
    else {
        T r = a % b;
        T g = exgcd(b, r, y, x);
        y -= (a / b) * x;
        return g;
    }
}

template <typename T> inline T inverse(T a, T m)
{
    T x, y;
    exgcd(a, m, x, y);
    return (x % m + m) % m;
}

const int K = 3;
mpz_class g[1 << K];

inline mpz_class pow_sliding_window(mpz_class a, mpz_class n, mpz_class m)
{
    g[1] = a, g[2] = (a * a) % m;
    for (int i = 1; i < (1 << (K - 1)); ++i) {
        g[(i << 1) + 1] = (g[(i << 1) - 1] * g[2]) % m;
    }
    auto e   = n.get_mpz_t();
    int  i   = mpz_sizeinbase(e, 2);
    mpz_class  res = 1;
    while (i >= 0) {
        if (!mpz_tstbit(e, i)) {
            res = (res * res) % m;
            --i;
        }
        else {
            int s = max(i - K + 1, 0);
            while (!mpz_tstbit(e, s)) ++s;
            for (int h = i - s; h >= 0; --h) res = (res * res) % m;
            int u = 0;
            for (int j = s; j <= i; ++j) u = (u << 1) + mpz_tstbit(e, j);
            res = (res * g[u]) % m;
            i   = s - 1;
        }
    }
    return res;
}

void Montgomery(){
    int n;
    mpz_class p, q, e;
    cin >> n >> p >> q >> e;
    mpz_class pq = p * q, pq1 = (p - 1) * (q - 1);
    mpz_class d  = inverse(e, pq1);
    mpz_class p1 = inverse(p, q), q1 = inverse(q, p);
    for (int it = 1; it <= n; ++it) {
        mpz_class c;
        cin >> c;
        mpz_class dp = d % (p - 1), dq = d % (q - 1);
        mpz_class cp = pow_sliding_window(c, dp, p),
            cq = pow_sliding_window(c, dq, q);
        cout << (cp * q1 * q + cq * p1 * p) % pq << endl;
    }
}
int main()
{
    Montgomery();
}