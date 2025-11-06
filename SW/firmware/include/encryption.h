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

/* Functions */
std::vector<uint8_t> get_encrypted_vector(const char *plaintext);
String encrypt_data(const char *plainTextID);

#endif // ENCRYPTION_H