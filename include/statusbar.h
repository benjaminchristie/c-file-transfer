#pragma once

#include <stdio.h>
#include <string.h>
// stolen from
// https://stackoverflow.com/questions/190229/where-is-the-itoa-function-in-linux
void reverse(char s[]) {
    int i, j;
    char c;

    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}
/* itoa:  convert n to characters in s */
void itoa(int n, char s[]) {
    int i, sign;

    if ((sign = n) < 0) /* record sign */
        n = -n;         /* make n positive */
    i = 0;
    do {                       /* generate digits in reverse order */
        s[i++] = n % 10 + '0'; /* get next digit */
    } while ((n /= 10) > 0);   /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}

int numDigits(int number) {
    int digits = 0;
    if (number < 0)
        digits = 1;
    while (number) {
        number /= 10;
        digits++;
    }
    return digits;
}

void statusbar(int x, int y, char* str) {
    printf(str, x, y);
    /* printf("\rBlocks: %0n_digitsd / %d", x, y); */
    return;
}
