/*********************************************************************
* Filename:   des.h
* Author:     Brad Conte (brad AT bradconte.com)
* Copyright:
* Disclaimer: This code is presented "as is" without any guarantees.
* Details:    Defines the API for the corresponding DES implementation.
              Note that encryption and decryption are defined by how
              the key setup is performed, the actual en/de-cryption is
              performed by the same function.
*********************************************************************/

#ifndef DES_H
#define DESH

/*************************** HEADER FILES ***************************/
#include <stddef.h>
#include "system.h"
/****************************** MACROS ******************************/
#define DES_BLOCK_SIZE 8                // DES operates on 8 bytes at a time

/**************************** DATA TYPES ****************************/
typedef enum {
	ENCRYPT,
	DECRYPT
} CryptoMode_enum;

/*********************** FUNCTION DECLARATIONS **********************/
void des_key_setup(const uint8_t key[], uint8_t schedule[][6], CryptoMode_enum mode);
void des_crypt(const uint8_t in[], uint8_t out[], const uint8_t key[][6]);

void three_des_key_setup(const uint8_t key[], uint8_t schedule[][16][6], CryptoMode_enum mode);
void three_des_crypt(const uint8_t in[], uint8_t out[], const uint8_t key[][16][6]);

#endif   // DES_H
