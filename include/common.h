#pragma once

#include <netinet/in.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <fcntl.h>
#include <openssl/ssl.h>
#include <sodium.h>
#include <statusbar.h>
#include <sys/mman.h>
#include <sys/sendfile.h>
#include <sys/stat.h>

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
	}
    while (strncmp(s, ACK, ACK_LEN));
    memset(s, 0, ACK_LEN);
}
