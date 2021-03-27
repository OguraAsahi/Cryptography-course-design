#include <cstdio>
#include <iostream>

using namespace std;


inline int c2i(char c)
{
    return c <= '9' ? c - '0' : c - 'a' + 10;
}

inline int read16()
{
    int  x = 0;
    char c = getchar();
    while (!isalnum(c)) c = getchar();
    while (isalnum(c)) {
        x = (x << 4) + c2i(c);
        c = getchar();
    }
    return x;
}

const int NR = 4, L = 4, M = 4, LM = 16;
const int S[LM]  = {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7};
const int P[LM]  = {1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15, 4, 8, 12, 16};
const int S1[LM] = {14, 3, 4, 8, 1, 12, 10, 15, 7, 13, 9, 6, 11, 2, 0, 5};
const int P1[LM] = {1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15, 4, 8, 12, 16};
uint16_t       K[NR + 1];

inline uint16_t permutation(uint16_t x, const int* P)
{
    uint16_t w = 0;
    for (int j = 0; j < LM; ++j) { w = (w << 1) + (x >> (LM - P[j]) & 1); }
    return w;
}

inline uint16_t substitution(uint16_t x, const int* S)
{
    uint16_t v = 0;
    for (int j = 0; j < M; ++j) { v |= S[(x >> (j * L)) & 15] << (j * L); }
    return v;
}

inline uint16_t spn_encrypt(uint16_t plain, uint32_t key)
{
    uint16_t u, v, w = plain;
    for (int i = 0; i <= NR; ++i) { K[i] = key >> (16 - (i << 2)); }
    for (int i = 0; i < NR - 1; ++i) {
        u = w ^ K[i];
        v = substitution(u, S);
        w = permutation(v, P);
    }
    u = w ^ K[NR - 1];
    v = substitution(u, S);
    return v ^ K[NR];
}

inline uint16_t spn_decrypt(uint16_t cipher, uint32_t key)
{
    uint16_t u, v, w = substitution(cipher ^ K[NR], S1) ^ K[NR - 1];
    for (int i = NR - 2; i >= 0; --i) {
        v = permutation(w, P1);
        u = substitution(v, S1);
        w = u ^ K[i];
    }
    return w;
}

int main()
{
    int n = 0;
    scanf("%d", &n);
    while (n--) {
        uint32_t key    = read16();
        uint16_t plain  = read16();
        uint16_t cipher = spn_encrypt(plain, key);
        printf("%04x %04x\n", cipher, spn_decrypt(cipher ^ 1, key));
    }
    return 0;
}