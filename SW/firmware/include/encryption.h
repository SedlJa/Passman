#ifndef ENCRYPTION_H
#define ENCRYPTION_H

/* Includes */
#include "mbedtls/aes.h"
#include "mbedtls/base64.h"
#include <vector>
#include <cstring>

/* Defines */

/* Variables */
const char SECRET_KEY[] = "MySecretKey12345";

/* Functions */
std::vector<uint8_t> encrypt_data(const char *plaintext);

#endif // ENCRYPTION_H