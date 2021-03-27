#include <cstdio>
#include <gmpxx.h>
#include <utility>

using namespace std;

inline void gcd(mpz_t g, mpz_t x, mpz_t y)
{
    mpz_t m, n, r;
    mpz_inits(m, n, r);
    mpz_set(m, x);
    mpz_set(n, y);
    while (mpz_cmp_ui(n, 0)) {
        mpz_mod(r, m, n);
        mpz_set(m, n);
        mpz_set(n, r);
    }
    *g = std::move(*m);
    mpz_clears(m, n, r);
}

int main()
{
    int n;
    scanf("%d", &n);
    while (n--) {
        mpz_t e, p, q, phi_n;
        mpz_inits(e, p, q, phi_n);
        mpz_inp_str(e, stdin, 10);
        mpz_inp_str(p, stdin, 10);
        mpz_inp_str(q, stdin, 10);
        // mpz_sub_ui()
        // mpz_mul(phi_n, )
        gcd(e, p, q);
        mpz_out_str(stdout, 10, e);
        mpz_clears(e, p, q);
    }
    return 0;
}