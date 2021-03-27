#include <cstdio>
#include <gmpxx.h>
#include <iostream>
#include <utility>

using namespace std;
const int BOUND = 1e4;

template <typename T> inline T GCD(T m, T n)
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

inline bool is_prime(const mpz_class& x)
{
    return mpz_probab_prime_p(x.get_mpz_t(), 20) >= 1;
}

int main()
{
    int n;
    cin >> n;
    for (int it = 1; it <= n; ++it) {
        mpz_class e, p, q;
        cin >> e >> p >> q;
        mpz_class p1 = p - 1, q1 = q - 1;
        mpz_class phi_n = p1 * q1;
        if (e < 10 || !is_prime(p) || !is_prime(q) || GCD(phi_n, e) != 1 ||
            abs(p - q) < BOUND || GCD(p1, q1) > 50) {
            cout << "ERROR\n";
        }
        else {
            cout << inverse(e, phi_n) << endl;
        }
    }
    return 0;
}