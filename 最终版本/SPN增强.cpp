#include <stdio.h>
#include <stdlib.h>
#include <time.h>

    int s[16] = { 14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7 };
    int p[64] = {1,40,16,63,30,29,32,22,13,61,53,24,55,4,47,50,7,11,35,14,57,6,17,5,46,23,9,41,21,25,45,33,15,20,31,
                 28,18,39,59,38,27,48,36,51,58,37,42,10,52,62,19,49,60,26,2,43,56,8,34,12,54,3,44,64};



unsigned long long spn(unsigned long long x,unsigned long long *key)
{
    unsigned long long temp = x,v,m;
    int i,j;
    for(i = 0; i < 3; i++)
    {
        temp ^= key[i];
        m = 0;
        for(j = 0; j < 16; j++)
        {
            v = (temp >> (60 - 4*j))&0xf;
            v = s[v];
            v = v << (60 - 4*j);
            m |= v;
        }
        temp = m;
        m = 0;
        for(j = 0; j < 64; j++)
        {
            v = ((temp >> (63 - j))&1) << (64 - p[j]);
            m |= v;
        }
        temp = m;
    }
    temp ^= key[3];
    m = 0;
    for(j = 0; j < 16; j++)
    {
        v = (temp >> (60 - 4*j))&0xf;
        v = s[v];
        v = v << (60 - 4*j);
        m |= v;
    }
    temp = m;
    temp ^= key[4];
    return temp;
}

void SPN_PLUS(){
    unsigned long long k[2],x,y=0;
    fread(k,sizeof(unsigned long long),2,stdin);
    unsigned long long key[5];
    key[0] = k[0];
    key[1] = (k[0] << 16) | (k[1] >> 48);
    key[2] = (k[0] << 32) | (k[1] >> 32);
    key[3] = (k[0] << 48) | (k[1] >> 16);
    key[4] = k[1];
    int i;
    srand(time(NULL));
    for (i = 0; i < 64; i++)
    {
        y <<= 1;
        y |= (rand() & 0x0000000000000001);
    }
    for(i = 0; i < (1 << 21); i++)
    {
        fread(&x,sizeof(unsigned long long),1,stdin);
        x=x^y;
        y=spn(x,key);
        fwrite(&y,sizeof(unsigned long long),1,stdout);
    }
}

int main()
{
    SPN_PLUS();
}