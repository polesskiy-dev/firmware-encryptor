# DES/3DES firmware encryptor and decryptor.  

Allow users to encrypt or decrypt their firmware for microcontrollers by DES or 3DES crypto.

## Input/Output format:

*.hex or *.elf.

Developed on ANSI C.
IDE: JetBrains CLion

# Commandline arguments:

    1. Plain firmware filename
    2. Encrypting firmware filename
    3. Crypto mode, can be 0-3: 
	    0:TDES_ENCRYPTION, 
	    1:TDES_DECRYPTION, 
	    2:DES_ENCRYPTION, 
	    3:DES_DECRYPTION 
    4. KEY (length 8 for DES or 24 for TDES)