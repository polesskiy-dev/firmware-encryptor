//
// Created by Max on 16.02.2016.
//

#ifndef ENCRYPTORTEST_SYSTEM_H
#define ENCRYPTORTEST_SYSTEM_H

#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

//default TDES key
#define TDES_DEFAULT_KEY                "TESTTESTTESTTESTTESTTEST"

//commandline arguments constants
#define NO_COMMANDLINE_ARGS             1
#define VALID_COMMANDLINE_ARGS_AMOUNT   5

#define EMPTY_FLASH_BYTE                0xFF

typedef enum {
    TDES_ENCRYPTION,
    TDES_DECRYPTION,
    DES_ENCRYPTION,
    DES_DECRYPTION
} CryptoMode;

static const char *cryptoMode_strings[] = {"TDES_ENCRYPTION", "TDES_DECRYPTION", "DES_ENCRYPTION", "DES_DECRYPTION"};

#endif //ENCRYPTORTEST_SYSTEM_H
