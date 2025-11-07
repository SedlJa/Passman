#ifndef ENCRYPTION_H
#define ENCRYPTION_H

/* Includes */
#include "mbedtls/aes.h"
#include "mbedtls/base64.h"
#include <vector>
#include <cstring>
#include <Arduino.h>

/* Defines */

/* Variables */
const char SECRET_KEY[] = "MySecretKey12345";
const char AES_BLOCK_SIZE = 16;
const size_t KEY_BITS = 128;

/* Functions */
String encrypt_data(const char *plaintext);
String decrypt_data(const String &base64_encrypted_data);

#endif // ENCRYPTION_H