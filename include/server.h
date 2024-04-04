#pragma once

#include <common.h>


int send_post_request(int sck, char* fn);
int encrypt_packet(packet* p, unsigned char* payload, long payload_len);
