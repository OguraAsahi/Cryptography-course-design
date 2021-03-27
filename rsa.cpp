#include <cstdio>
#include <gmpxx.h>
#include <iostream>
#include <utility>

using namespace std;

typedef mpz_class Int;

template <typename T> inline T gcd(T m, T n)
{
    while (n != 0) {
        T r = m % n;
        m   = n;
        n   = r;
    }
    return m;
}

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

int main()
{
    ios::sync_with_stdio(0);
    int n;
    cin >> n;
    while (n--) {
        Int e, p, q;
        cin >> e >> p >> q;
        Int phi_n = (p - 1) * (q - 1);
        cout << inverse(e, phi_n) << endl;
    }
    return 0;
}