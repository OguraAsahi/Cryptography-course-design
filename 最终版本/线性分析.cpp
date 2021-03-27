#include <cstdio>
#include <cstring>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdint>

using namespace std;
const int T = 8000, MAXM = 65536;
const int NR = 4, L = 4, M = 4, LM = 16;


inline int c2i(char c) {
    return c <= '9' ? c - '0' : c - 'a' + 10;
}

inline int read16() {
    int x = 0;
    char c = getchar();
    while (!isalnum(c))
        c = getchar();
    while (isalnum(c)) {
        x = (x << 4) + c2i(c);
        c = getchar();
    }
    return x;
}



const int SBox[] = { 0xE,0x4,0xD,0x1,0x2,0xF,0xB,0x8,0x3,0xA,0x6,0xC,0x5,0x9,0x0,0x7 };
const int PBox[] = { 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15, 4, 8, 12, 16 };
const int S_1[] = { 14, 3, 4, 8, 1, 12, 10, 15, 7, 13, 9, 6, 11, 2, 0, 5 };
unsigned short  K[NR + 1];
unsigned short TS[MAXM], TP[MAXM];


inline unsigned short permutation(unsigned short x, const int* P)
{
    unsigned short w = 0;
    for (int j = 0; j < LM; ++j) { w = (w << 1) + (x >> (LM - PBox[j]) & 1); }
    return w;
}

inline unsigned short substitution(unsigned short x, const int* S)
{
    unsigned short v = 0;
    for (int j = 0; j < M; ++j) { v |= SBox[(x >> (j * L)) & 15] << (j * L); }
    return v;
}

inline unsigned int spn_encrypt(unsigned short plain, unsigned int key)
{
    unsigned int w = plain, v = 0, kr = 0, u = 0;
    for (int i = 0; i < 3; i++) {
        kr = (key >> (16 - (i << 2))) & 0xFFFF;
        u = w ^ kr;
        v = TS[u];
        w = TP[v];
    }
    kr = (key >> 4) & 0xFFFF;
    u = w ^ kr;
    v = TS[u];
    kr = key & 0xFFFF;
    u = v ^ kr;
    return u;
}


struct Node
{
    unsigned int  x, y;
    unsigned int  cnt = 0;  
    bool operator<(const Node& rhs) const
    {
        return cnt > rhs.cnt;
    }
};

void SPN_Linear(){
    
    int cnt1[LM][LM] = {}, cnt2[LM][LM] = {}, cnt3[LM][LM] = {}, cnt4[LM][LM] = {};
	unsigned int n;
	unsigned short plains[T], ciphers[T];
    scanf("%d", &n);
    for (int i = 0; i < MAXM; ++i)
        TS[i] = substitution(i, SBox), TP[i] = permutation(i, PBox);
    while (n--) {
        Node L13[256], L24[256];
        memset(cnt2, 0, sizeof(cnt2));
        memset(cnt4, 0, sizeof(cnt4));
        for (int i = 0; i < T; ++i) plains[i] = read16(), ciphers[i] = read16();
        unsigned int ans = 0;
        for (int i = 0; i < T; ++i) {
            unsigned int x = plains[i], y = ciphers[i];
            int y2 = (y & 0x0f00) >> 8, y4 = (y & 0x000f);  
            int x5 = (x & 0x0800) >> 11, x7 = (x & 0x0200) >> 9,
                x8 = (x & 0x0100) >> 8, x13 = (x & 0x0008) >> 3,
                x15 = (x & 0x0002) >> 1;  
            int v_1 = 0, v_2 = 0, u_1 = 0, u_2 = 0;
            int u6 = 0, u7 = 0, u8 = 0, u14 = 0, u15 = 0, u16 = 0;
            int z = 0;
            for (int L2 = 0; L2 < 16; ++L2)
                for (int L4 = 0; L4 < 16; ++L4) { 
                    v_1 = L2 ^ y2;
                    v_2 = L4 ^ y4;
                    u_1 = S_1[v_1];
                    u_2 = S_1[v_2];
                    u6 = (u_1 & 4) >> 2;
                    u8 = u_1 & 1;
                    u14 = (u_2 & 4) >> 2;
                    u16 = u_2 & 1;
                    u7 = (u_1 & 2) >> 1;
                    u15 = (u_2 & 2) >> 1;
                    z = x5 ^ x7 ^ x8 ^ u6 ^ u8 ^ u14 ^ u16;
                    if (!z) ++cnt2[L2][L4];
                    z = x13 ^ x15 ^ u7 ^ u15;
                    if (!z) ++cnt4[L2][L4];
                }
        }
        for (int i = 0; i < 16; ++i)
            for (int j = 0; j < 16; ++j) {
                cnt2[i][j] = abs(cnt2[i][j] - T / 2);
                cnt4[i][j] = abs(cnt4[i][j] - T / 2);
            }
        int t = 0;
        for (int i = 0; i < 16; ++i)
            for (int j = 0; j < 16; ++j) {
                L24[t].x = i;
                L24[t].y = j;
                L24[t++].cnt = cnt4[i][j] + cnt2[i][j];
            }
        sort(L24, L24 + 256);
        bool flag = 0;
        for (int N = 0; N < 256; ++N) { 
            if (flag) break;
            memset(cnt1, 0, sizeof(cnt1));
            memset(cnt3, 0, sizeof(cnt3));
            for (int i = 0; i < T; i++) {
                int x = plains[i], y = ciphers[i];
                int y1 = y >> 12, y2 = (y & 0x0f00) >> 8,
                    y3 = (y & 0x00f0) >> 4, 
                    y4 = (y & 0x000f);
                int x1 = (x & 0x8000) >> 15, x2 = (x & 0x4000) >> 14,
                    x4 = (x & 0x1000) >> 12, x9 = (x & 0x0080) >> 7,
                    x10 = (x & 0x0040) >> 6,
                    x12 = (x & 0x0010) >> 4;  
                int v_1 = 0, v_2 = 0, v_3 = 0, v_4 = 0, u_1 = 0, u_2 = 0,
                    u_3 = 0, u_4 = 0;
                int u1 = 0, u3 = 0, u11 = 0, u5 = 0, u7 = 0, u9 = 0, u13 = 0,
                    u15 = 0;
                int z = 0;
                v_2 = L24[N].x ^ y2;
                v_4 = L24[N].y ^ y4;
                u_2 = S_1[v_2];
                u_4 = S_1[v_4];
                u5 = u_2 >> 3;
                u13 = u_4 >> 3;
                u7 = (u_2 & 2) >> 1;
                u15 = (u_4 & 2) >> 1;
                for (int L1 = 0; L1 < 16; ++L1)
                    for (int L3 = 0; L3 < 16; ++L3) {
                        v_1 = L1 ^ y1;
                        v_3 = L3 ^ y3;
                        u_1 = S_1[v_1];
                        u_3 = S_1[v_3];
                        u1 = u_1 >> 3;
                        u9 = u_3 >> 3;
                        u3 = (u_1 & 2) >> 1;
                        u11 = (u_3 & 2) >> 1;
                        z = x1 ^ x2 ^ x4 ^ u1 ^ u5 ^ u9 ^ u13;
                        if (!z) ++cnt1[L1][L3];
                        z = x9 ^ x10 ^ x12 ^ u3 ^ u7 ^ u11 ^ u15;
                        if (!z) ++cnt3[L1][L3];
                    }
            }
            for (int i = 0; i < 16; i++)
                for (int j = 0; j < 16; j++) {
                    cnt1[i][j] = abs(cnt1[i][j] - 4000);
                    cnt3[i][j] = abs(cnt3[i][j] - 4000);
                }
            int t = 0;
            for (int i = 0; i < 16; i++)
                for (int j = 0; j < 16; j++) {
                    L13[t].x = i;
                    L13[t].y = j;
                    L13[t++].cnt = cnt1[i][j] + cnt3[i][j];
                }
            sort(L13, L13 + 256);
            for (int j = 0; j < 2; j++) {
                if (flag) break;
                unsigned int k = 0;
                k |= L13[j].x;
                k <<= 4;
                k |= L24[N].x;
                k <<= 4;
                k |= L13[j].y;
                k <<= 4;
                k |= L24[N].y;
                for (int l = 0; l < 65536; ++l) { 
                    unsigned int key = k | (l << 16);
                    if (spn_encrypt(plains[1], key) == ciphers[1] &&
                        spn_encrypt(plains[2], key) == ciphers[2]) {
                        flag = 1;
                        ans = key;
                    }
                }
            }
        }
        printf("%08x\n", ans);
    }
}

int main()
{
  	SPN_Linear();
}