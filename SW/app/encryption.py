from Crypto.Cipher import AES
from Crypto.Util.Padding import pad, unpad
from Crypto.Random import get_random_bytes
import base64

# --- Shared Secret Key ---
# Must be identical on both the ESP32 and Python app.
# 16 bytes for AES-128
SECRET_KEY = b'MySecretKey12345'


def decrypt_data(encrypted_data_b64):
    """
        Decrypt encrypted data in base64 format
        Args:
            encrypted_data_b64:
                input encrypted data
        Return:
            plaintext:
                decrypted output text
    """
    # 1. Decode the Base64 string to bytes
    encrypted_data = base64.b64decode(encrypted_data_b64)

    # 2. Separate the IV and the Ciphertext
    # We assume the first 16 bytes are the IV
    IV_LENGTH = 16 
    iv = encrypted_data[:IV_LENGTH]
    ciphertext = encrypted_data[IV_LENGTH:]

    # 3. Create the AES cipher object
    cipher = AES.new(SECRET_KEY, AES.MODE_CBC, iv)
    
    # 4. Decrypt and remove padding
    decrypted_padded_data = cipher.decrypt(ciphertext)
    plaintext = unpad(decrypted_padded_data, AES.block_size).decode('utf-8')

    return plaintext


def encrypt_data(plaintext):
    """
        Encrypt plain text to base64 hash
        Args:
            plaintext:
                decrypted output text
            
        Return:
            encrypted_data_b64:
                encrypted base64 hash
    """

    # Generate a unique, random IV
    iv = get_random_bytes(16)
    
    # Create the cipher object
    cipher = AES.new(SECRET_KEY, AES.MODE_CBC, iv)
    
    # Pad the plaintext
    padded_data = cipher.encrypt(pad(plaintext.encode('utf-8'), AES.block_size))
    
    # Combine IV and Ciphertext, then Base64 encode for easy transmission
    combined_data = iv + padded_data
    return base64.b64encode(combined_data).decode('utf-8')

print("\n--- Python Encryption (to send back to ESP32) ---")
plaintext_to_send = "1;sedlaj51;silneHeslo"
encrypted_to_send = encrypt_data(plaintext_to_send)
print(f"Plaintext: {plaintext_to_send}")
print(f"Encrypted (Base64): {encrypted_to_send}")
print("\n--- Python Decryption (received from ESP32) ---")
decrypted_received = decrypt_data(b"Lc9GKQS0eNhop/8/K/H82Yl8KAzYKjNGMz27u9y8b+o=")
print(f"Decrypted Plaintext: {decrypted_received}")