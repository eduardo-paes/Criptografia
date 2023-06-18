#include <stdio.h>
#define uchar unsigned char 
uchar SBOX[16] = { 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7 };

uchar bit( uchar x, uchar i ) {
    return (x >> (4-i)) & 1;
}

void genBiasTable() {
    int c = 0;
    printf("    ");
    for (uchar x = 1; x < 16; x++) {
        printf("%2X ", x);
    }
    printf("\n");

    for (uchar x = 1; x < 16; x++) {
        printf("%2X  ", x);
        for (uchar y = 1; y < 16; y++) {
            c = 0;
            for (uchar X = 0; X < 16; X++) {
                int a;
                if ((a = ((bit(X, 1) & bit(x, 1)) ^
                          (bit(X, 2) & bit(x, 2)) ^
                          (bit(X, 3) & bit(x, 3)) ^
                          (bit(X, 4) & bit(x, 4)) ^
                          (bit(SBOX[X], 1) & bit(y, 1)) ^
                          (bit(SBOX[X], 2) & bit(y, 2)) ^
                          (bit(SBOX[X], 3) & bit(y, 3)) ^
                          (bit(SBOX[X], 4) & bit(y, 4)))) == 0)
                    c++;
            }
            printf("%2d ", c - 8);
        }
        printf("\n");
    }
}

uchar encrypt( uchar w, uchar k ) {
    uchar x = w ^ k;
    uchar y = SBOX[x];
    return y;
}

int main() {
    int c = 0;
    uchar Y = 0;
    for( uchar X = 0; X < 16; X++ ) {
        Y = SBOX[X];
        if( bit(X, 3)^ bit(X, 4)^ bit(Y, 1)^bit(Y, 4) == 1 )       
            c++;
    }
    printf("c = %d\n", c);
    genBiasTable();
  
  
    for (uchar W = 0; W < 16; W++ ) {
        for (uchar K = 7; K < 8; K++ ) {
            Y = encrypt(W,K);
            if ((bit(K,3) ^ bit(K,4)) == (bit(W,3) ^ bit(W,4) ^ bit(Y,1) ^ bit(Y,4) ^ 1)) {
                c++;
            }        
        }
    } 
    printf("%f\n",c/(16.)); 
    return 0;
}
