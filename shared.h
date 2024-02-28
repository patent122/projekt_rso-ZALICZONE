#pragma once

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 8889

// Odwrócona kolejność bajtów dla LE
#define SQUARE_ROOT_REQUEST_TYPE  0x01000000
#define SQUARE_ROOT_RESPONSE_TYPE 0x01000001
#define TIME_REQUEST_TYPE         0x02000000
#define TIME_RESPONSE_TYPE        0x02000001

void invert_endianness(void* buffer, int n);
