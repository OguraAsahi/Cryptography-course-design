#include <cstdio>
#include <iostream>
#include <gmpxx.h>
#include <utility>

using namespace std;


inline mpz_class pow_mod(mpz_class a, mpz_class n, mpz_class m)
{
    mpz_class res = 1;
    while (n) {
        if (n % 2 == 1) res = res * a % m;
        a = (a * a) % m;
        n >>= 1;
    }
    return res;
}

int main()
{
    int n;
    cin >> n;
    for (int it = 1; it <= n; ++it) {
        mpz_class e, m, p, q;
        cin >> e >> m >> p >> q;
        cout << pow_mod(m, e, p * q) << endl;
    }
    return 0;
}