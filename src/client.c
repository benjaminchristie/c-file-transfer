#include <client.h>
#include <sodium.h>
#include <unistd.h>

int decrypt_packet(packet* p, unsigned char* payload, long payload_len,
                   unsigned char* tag) {

    if (crypto_secretstream_xchacha20poly1305_pull(&(p->state), payload, NULL,
                                                   tag, p->payload, CRYPT_SIZE,
                                                   NULL, 0) != 0) {
        return 1;
    }
    return 0;
}

int read_post_request(int sck, char* fn) {
    FILE* fptr;
    long fbytes;
    struct packet p;
    int int_width;
    long read_size;
    unsigned char* key;
    char conf[2] = "0";
    unsigned char tag;
    unsigned char byte_packet[HEADER_BYTES + CRYPT_SIZE] = {0};
    unsigned char buffer[BUF_READ_SIZE + 1] = {0};
    char print_str[256] = {0};

    key = (unsigned char*)getenv("USER_SOCKET_KEY");
    if (key == NULL) {
        return 2;
    }

    read(sck, buffer, sizeof(long));

    fbytes = *((long*)&buffer);
    int_width = numDigits(fbytes);
    strcpy(print_str, "\rBlocks: %0");
    itoa(int_width, print_str + 11);
    strcpy(print_str + strlen(print_str), "d / %d");
    printf("\n");

    send(sck, ACK, ACK_LEN, 0); // confirm

    // get first packet
    read(sck, byte_packet, HEADER_BYTES + CRYPT_SIZE);
    memcpy(p.header, byte_packet, HEADER_BYTES);
    if (crypto_secretstream_xchacha20poly1305_init_pull(&p.state, p.header,
                                                        key) != 0) {
        printf("Invalid header, aborting\n");
        return 1;
    }
    fptr = fopen(fn, "wb");
    for (int i = 0; i < fbytes; i += BUF_READ_SIZE) {

        memcpy(p.payload, byte_packet + HEADER_BYTES, CRYPT_SIZE);
        if (decrypt_packet(&p, buffer, BUF_READ_SIZE, &tag) != 0) {
            printf("Decryption failed! Bailing now\n");
            fclose(fptr);
            return 1;
        }

        read_size = min(BUF_READ_SIZE, fbytes - i);
        fwrite(buffer, read_size, 1, fptr);
        statusbar(i, fbytes, print_str);

        send(sck, ACK, ACK_LEN, 0); // confirm

        read(sck, byte_packet, HEADER_BYTES + CRYPT_SIZE);
    }

    send(sck, ACK, ACK_LEN, 0); // confirm
    fclose(fptr);

    return 0;
}

int main(int argc, char** argv) {
    int sck;
    int bind_success;
    int status;
    char buffer[256] = {0};
    char req[256] = "GET /";
    char ip_s[256] = {0};
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);

    if ((sck = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }

    printf("Enter IP to connect to: ");
    fgets(ip_s, 256, stdin);
    strtok(ip_s, "\n");

    if (inet_pton(AF_INET, ip_s, &addr.sin_addr) <= 0) {
        printf("Address [%s] not supported\n", ip_s);
        return -1;
    }

    if ((status = connect(sck, (struct sockaddr*)&addr, sizeof(addr))) < 0) {
        printf("Connection Failed\n");
        return -1;
    }

    printf("Enter filename: ");
    fgets(req + 5, 251, stdin);
    *strchr(req + 5, '\n') = 0;
    send(sck, req, 256, 0);
    printf("Sent request:\n \
			--------------\n \
			%s\n\
			--------------\n",
           req);

    // begin post request reading

    read_post_request(sck, req + 5);

    close(sck);

    return 0;
}
