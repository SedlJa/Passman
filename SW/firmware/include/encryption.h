#ifndef ENCRYPTION_H
#define ENCRYPTION_H

/* Includes */
#include "mbedtls/aes.h"
#include "mbedtls/base64.h"
#include <vector>
#include <cstring>
#include <Arduino.h>

/* Defines */
const size_t AES_BLOCK_SIZE = 16;
const size_t AES_KEY_SIZE = 16;
const size_t KEY_BITS = 128;

/* Functions */
String encrypt_data(const char *plaintext);
String decrypt_data(const String &base64_encrypted_data);
void print_secret_key_debug(); // just for debug - DO NOT USE

#endif // ENCRYPTION_H
