/* Includes */
#include "encryption.h"

/* Defines */

/* Functions */
std::vector<uint8_t> get_encrypted_vector(const char *plaintext)
{
    mbedtls_aes_context aes_ctx;
    size_t plaintext_len = strlen(plaintext);

    // 1. Generate a new, random IV (16 bytes)
    uint8_t iv[16];
    // Using a simple method for example. For production, use a secure RNG like esp_fill_random()
    for (int i = 0; i < 16; i++)
    {
        iv[i] = random();
    }

    // 2. Pad the plaintext (PKCS7/PKCS5 padding)
    size_t padded_len = plaintext_len + (16 - (plaintext_len % 16));
    std::vector<uint8_t> padded_plaintext(padded_len);
    memcpy(padded_plaintext.data(), plaintext, plaintext_len);

    // Apply PKCS7 padding (mbedTLS does not have a built-in pad/unpad function for this mode)
    uint8_t padding_value = padded_len - plaintext_len;
    for (size_t i = plaintext_len; i < padded_len; i++)
    {
        padded_plaintext[i] = padding_value;
    }

    // 3. Initialize the AES context
    mbedtls_aes_init(&aes_ctx);
    mbedtls_aes_setkey_enc(&aes_ctx, (const unsigned char *)SECRET_KEY, 128);

    // 4. Encrypt
    std::vector<uint8_t> ciphertext(padded_len);
    uint8_t iv_copy[16];
    memcpy(iv_copy, iv, 16); // IV must be copied for mbedtls_aes_crypt_cbc

    if (mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_ENCRYPT, padded_len, iv_copy, padded_plaintext.data(), ciphertext.data()) != 0)
    {
        mbedtls_aes_free(&aes_ctx);
        return {};
    }

    mbedtls_aes_free(&aes_ctx);

    // 5. Combine IV and Ciphertext for transmission
    std::vector<uint8_t> final_payload;
    final_payload.insert(final_payload.end(), iv, iv + 16);                          // Add IV
    final_payload.insert(final_payload.end(), ciphertext.begin(), ciphertext.end()); // Add Ciphertext

    return final_payload;
}

String encrypt_data(const char *plainTextID)
{
    std::vector<uint8_t> encrypted_payload = get_encrypted_vector(plainTextID);

    if (!encrypted_payload.empty())
    {
        // Convert the final IV + Ciphertext payload to Base64
        size_t output_len;
        mbedtls_base64_encode(NULL, 0, &output_len, encrypted_payload.data(), encrypted_payload.size());
        std::vector<unsigned char> base64_buf(output_len);
        mbedtls_base64_encode(base64_buf.data(), base64_buf.size(), &output_len, encrypted_payload.data(), encrypted_payload.size());

        return String((char *)base64_buf.data());
    }
    else
    {
        Serial.println("Encryption failed!");
        return String();
    }
}