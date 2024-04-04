#include <common.h>
#include <server.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int encrypt_packet(packet* p, unsigned char* payload, long payload_len) {

    return crypto_secretstream_xchacha20poly1305_push(
        &p->state, p->payload, NULL, payload, payload_len, NULL, 0, 0);
}

int send_post_request(int sck, char* fn) {

    FILE* fptr;
    char* key;
    int offset;
    long fsize;
    struct stat f_stat_data;
    packet p;
    unsigned char buffer[BUF_READ_SIZE + 1] = {0};
    unsigned char byte_packet[HEADER_BYTES + CRYPT_SIZE] = {0};
    char print_str[256] = {0};
    char ack_buffer[ACK_LEN] = {0};

    if (stat(fn, &f_stat_data) != 0) {
        printf("File does not exist on server\n");
        return 1;
    }
    fsize = f_stat_data.st_size;
    fptr = fopen(fn, "rb");

    key = getenv("USER_SOCKET_KEY");
    if (key == NULL) {
        return 2;
    }

    memcpy(buffer, &fsize, sizeof(fsize));
    send(sck, buffer, sizeof(fsize), 0);

    int int_width = numDigits(fsize);
    strcpy(print_str, "\rBlocks: %0");
    itoa(int_width, print_str + 11);
    strcpy(print_str + strlen(print_str), "d / %d");
    printf("\n");
    crypto_secretstream_xchacha20poly1305_init_push(&p.state, p.header,
                                                    (unsigned char*)key);
    for (offset = 0; offset < fsize; offset += BUF_READ_SIZE) {
        wait_ack(sck, ack_buffer);

        statusbar(offset, fsize, print_str);
        fread(buffer, BUF_READ_SIZE, 1, fptr);

        encrypt_packet(&p, buffer, BUF_READ_SIZE);
        memcpy(byte_packet, p.header, HEADER_BYTES);
        memcpy(byte_packet + HEADER_BYTES, p.payload, CRYPT_SIZE);

        send(sck, byte_packet, HEADER_BYTES + CRYPT_SIZE, 0);
    }
    printf("\n");
    fclose(fptr);

    return 0;
}

int main() {
    int sck;
    int bind_success;
    int client_fd;
    char* buffer = calloc(256, sizeof(char));
    char* fn;
    if (sodium_init() > 0) {
        return ERR_SODIUM_INIT;
    }
    const struct sockaddr_in addr = {
        AF_INET,
        htons(PORT),
        0,
    };
    sck = socket(AF_INET, SOCK_STREAM, 0);
    bind_success = bind(sck, (struct sockaddr*)&addr, sizeof(addr));
    if (bind_success != 0) {
        return bind_success;
    }
    listen(sck, 10);
    client_fd = accept(sck, 0, 0);
    /* signal(SIGINT, int_handler); */
    /* for (; spin;) { */
    printf("Awaiting POST request...");
    recv(client_fd, buffer, 256, 0);
    printf("Received %s\n", buffer);
    fn = buffer + 5;
    send_post_request(client_fd, fn);
    close(client_fd);
    close(sck);
    free(buffer);
    return 0;
}
