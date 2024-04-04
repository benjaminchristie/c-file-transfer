#pragma once

#include <common.h>

int read_post_request(int sck, char* fn);
int decrypt_packet(packet* p, unsigned char* payload, long payload_len, unsigned char* tag);
