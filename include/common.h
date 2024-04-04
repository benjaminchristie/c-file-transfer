#pragma once

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <sodium.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#define max(a, b)                                                              \
    ({                                                                         \
        __typeof__(a) _a = (a);                                                \
        __typeof__(b) _b = (b);                                                \
        _a > _b ? _a : _b;                                                     \
    })
#define min(a, b)                                                              \
    ({                                                                         \
        __typeof__(a) _a = (a);                                                \
        __typeof__(b) _b = (b);                                                \
        _a > _b ? _b : _a;                                                     \
    })

#define BUF_READ_SIZE 1024
#define HEADER_BYTES crypto_secretstream_xchacha20poly1305_HEADERBYTES
#define CRYPT_SIZE BUF_READ_SIZE + crypto_secretstream_xchacha20poly1305_ABYTES
#define PORT 8080

#define ERR_SODIUM_INIT 1
#define ERR_CLIENT_SUS 2
#define ERR_SERVER_SUS 3
#define ACK "<!Recieved!>"
#define ACK_LEN 12

struct packet {
    crypto_secretstream_xchacha20poly1305_state state;
    unsigned char header[HEADER_BYTES];
    unsigned char payload[CRYPT_SIZE];
};
typedef struct packet packet;

void wait_ack(int sck, char* s) {
    do {
        read(sck, s, ACK_LEN);
    } while (strncmp(s, ACK, ACK_LEN));
    memset(s, 0, ACK_LEN);
}

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
