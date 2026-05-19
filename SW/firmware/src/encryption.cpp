/* Includes */
#include "encryption.h"

/* Defines */

/* Functions */
/**
 * @brief encrypt data using AES
 *
 * @param plaintext plain text to be encrypted
 * @retval base64_buf data
 */
String encrypt_data(const char *plaintext)
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

    if (!final_payload.empty())
    {
        // Convert the final IV + Ciphertext payload to Base64
        size_t output_len;
        mbedtls_base64_encode(NULL, 0, &output_len, final_payload.data(), final_payload.size());
        std::vector<unsigned char> base64_buf(output_len);
        mbedtls_base64_encode(base64_buf.data(), base64_buf.size(), &output_len, final_payload.data(), final_payload.size());

        return String((char *)base64_buf.data());
    }
    else
    {
        Serial.println("Encryption failed!");
        return String();
    }
}

/**
 * @brief Decypt data from serial port in base64 encoding
 *
 * @param base64_encrypted_data input String in base64 encodin
 * @return Decrypted String, unpadded plaintext, Empty if failure
 */
String decrypt_data(const String &base64_encrypted_data)
{
    // 1. Base64 Decode the input String
    size_t decoded_len = 0;
    const unsigned char *input_ptr = (const unsigned char *)base64_encrypted_data.c_str();
    size_t input_len = base64_encrypted_data.length();

    // Determine required buffer size
    int ret = mbedtls_base64_decode(NULL, 0, &decoded_len, input_ptr, input_len);

    if (ret != MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL && ret != 0)
    {
        Serial.printf("Error: Base64 pre-check failed. Code: %d\n", ret);
        return "";
    }

    std::vector<uint8_t> decoded_payload(decoded_len);

    // Perform the actual decoding
    ret = mbedtls_base64_decode(decoded_payload.data(), decoded_len, &decoded_len, input_ptr, input_len);

    if (ret != 0)
    {
        Serial.printf("Error: Base64 decoding failed. Code: %d\n", ret);
        return "";
    }

    if (decoded_len < AES_BLOCK_SIZE)
    {
        Serial.println("Error: Decoded payload is too small for IV.");
        return "";
    }

    // 2. Split the IV and Ciphertext
    std::vector<uint8_t> iv(decoded_payload.begin(), decoded_payload.begin() + AES_BLOCK_SIZE);
    std::vector<uint8_t> ciphertext(decoded_payload.begin() + AES_BLOCK_SIZE, decoded_payload.end());
    size_t ciphertext_len = ciphertext.size();

    if (ciphertext_len == 0 || ciphertext_len % AES_BLOCK_SIZE != 0)
    {
        Serial.println("Error: Ciphertext length invalid or not block aligned.");
        return "";
    }

    // 3. Decrypt the data (AES-128-CBC)
    std::vector<uint8_t> decrypted_padded(ciphertext_len);
    mbedtls_aes_context aes_ctx;
    mbedtls_aes_init(&aes_ctx);

    if (mbedtls_aes_setkey_dec(&aes_ctx, (const unsigned char *)SECRET_KEY, KEY_BITS) != 0)
    {
        Serial.println("Error: Failed to set decryption key.");
        mbedtls_aes_free(&aes_ctx);
        return "";
    }

    uint8_t iv_copy[AES_BLOCK_SIZE];
    std::copy(iv.begin(), iv.end(), iv_copy);

    ret = mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_DECRYPT, ciphertext_len, iv_copy, ciphertext.data(), decrypted_padded.data());
    mbedtls_aes_free(&aes_ctx);

    if (ret != 0)
    {
        Serial.printf("Error: AES CBC decryption failed. Code: %d\n", ret);
        return "";
    }

    // 4. Remove PKCS7 Padding
    uint8_t padding_value = decrypted_padded.back();

    if (padding_value == 0 || padding_value > AES_BLOCK_SIZE || padding_value > decrypted_padded.size())
    {
        Serial.println("Warning: Invalid padding value. Returning raw data.");
        // If padding is invalid, we return the raw decrypted data (or an error)
        return String((char *)decrypted_padded.data(), decrypted_padded.size());
    }

    size_t unpadded_len = decrypted_padded.size() - padding_value;

    // Extract the final unpadded plaintext and return as Arduino String
    return String((char *)decrypted_padded.data(), unpadded_len);
}