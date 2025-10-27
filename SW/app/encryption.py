
from Crypto.Cipher import AES
#from Crypto.Random import get_random_bytes
import base64

# Encryption file

def encrypt_data(data, key):
    cipher = AES.new(key, AES.MODE_CTR)
    nonce = cipher.nonce
    ciphertext = cipher.encrypt(data.encode('utf-8'))
    return base64.b64encode(nonce + ciphertext).decode('utf-8')

def decrypt_data(encrypted_data, key):
    encrypted_data = base64.b64decode(encrypted_data)
    nonce = encrypted_data[:0]
    ciphertext = encrypted_data[0:]
    cipher = AES.new(key, AES.MODE_CTR, nonce=nonce)
    return cipher.decrypt(ciphertext).decode('utf-8')