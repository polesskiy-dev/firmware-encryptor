//
// Created by Max on 16.02.2016.
//

#include "hex_string.h"
#include "system.h"
#include "des.h"

#define FIRMWARE_PLAIN_FILENAME     "test_firmware.hex"
#define FIRMWARE_ENCRYPTED_FILENAME "result_firmware.hex"

/**************************** GLOBAL VARIABLES *****************************/
HEXstring_struct g_HexStringStruct_buffer;                      //buffer Intel HEX struct to parse data into it
char g_lineBuffer_ptr[HEX_LINE_ASCII_MAX_LENGTH];               //buffer for reading line from file into it

/*
 * Files (pointers to their streams)
 */
FILE *g_PlainFile_ptr;                                          //file to encrypt/decrypt
FILE *g_EncryptedFile_ptr;                                      //result file

/*
 * Crypto engine settings
 */
CryptoMode g_Des_mode;                                          //Mode: encryption or decryption
uint8_t *g_DES_KEY_ptr;                                           //keys for triple des

typedef void (*CryptoFunction)(uint8_t *data);                  //crypto function type

/**************************** FUNCTION PROTOTYPES *****************************/
/**
 * TDES crypto function
 * encrypts data and replaces plain
 */
void tDesPerformOn(uint8_t *plainData);

/**
 * DES crypto function
 * encrypts data and replaces plain
 */
void desPerformOn(uint8_t *plainData);

/**************************** PROGRAM ENTER POINT *****************************/
int main(int argc, char *argv[]) {
    //local variables
    char *plainFileName_ptr;
    char *encryptedFileName_ptr;

    /*
     * Parse commandline argvs
     * argvc - amound of arguments
     * arrgv[] - array of argument strings
     */
    switch (argc) {
        case NO_COMMANDLINE_ARGS:
            plainFileName_ptr = FIRMWARE_PLAIN_FILENAME;
            encryptedFileName_ptr = FIRMWARE_ENCRYPTED_FILENAME;
            g_Des_mode = TDES_ENCRYPTION;
            g_DES_KEY_ptr = TDES_DEFAULT_KEY;
            break;
        case VALID_COMMANDLINE_ARGS_AMOUNT:
            /*
            * Parse commandline argvs to values
            */
            plainFileName_ptr = argv[1];                //"in" firmware plain file
            encryptedFileName_ptr = argv[2];            //"out" firmware crypted file
            g_Des_mode = (CryptoMode) atoi(
                    argv[3]);                           //set mode from enum index 0:TDES_ENCRYPTION, 1:TDES_DECRYPTION,2:DES_ENCRYPTION,3:DES_DECRYPTION
            //debug
            //printf("des mode: %d\r\n", atoi(argv[3]));
            g_DES_KEY_ptr = argv[4];                    //des key*/
            break;
        default:
            printf("Error:Invalid commandline arguments amount\r\n");
            exit(EXIT_FAILURE);
    }

    /*
    * Validate commandline argvs data
    */
    if ((strlen(plainFileName_ptr) > 0) && (strlen(encryptedFileName_ptr) > 0) &&
        ((g_Des_mode >= 0) && (g_Des_mode <= 3)) && (strlen(g_DES_KEY_ptr) > 0)) {
    } else {
        printf("Error:Invalid commandline arguments data\r\n");
        exit(EXIT_FAILURE);
    }

    //show disclaimer
    printf("Firmware encryptor starts\r\nPlain firmware file:%s\r\nResult file:%s\r\nCrypto mode:%s\r\nKey:%s\r\n",
           plainFileName_ptr, encryptedFileName_ptr, cryptoMode_strings[g_Des_mode],
           g_DES_KEY_ptr);

    /*
     * Set crypto function
     * can be tdes or des
     */
    CryptoFunction crypto_function = (g_Des_mode == TDES_ENCRYPTION || (g_Des_mode == TDES_DECRYPTION))
                                     ? tDesPerformOn
                                     : desPerformOn;

    /*
     * Prepare files
     * open and check the success of opening
     */
    g_PlainFile_ptr = fopen(plainFileName_ptr, "r");                //open file with plain firmware data
    if (g_PlainFile_ptr == NULL) {                                  //if plain firmware file not found - exit
        printf("Plain firmware file \"%s\" not found\r\n", plainFileName_ptr);
        exit(EXIT_FAILURE);
    }

    g_EncryptedFile_ptr = fopen(encryptedFileName_ptr, "w");        //open or create file for encrypted firmware data
    if (g_EncryptedFile_ptr == NULL) {                              //if couldn't create file - exit
        printf("Couldn't create file \"%s\" \r\n", encryptedFileName_ptr);
        exit(EXIT_FAILURE);
    }

    /*
     * Read, perform crypto function and write to result file line-by-line
     */
    while (fgets(g_lineBuffer_ptr, HEX_LINE_ASCII_MAX_LENGTH, g_PlainFile_ptr)) {

        /*
         * Parse ASCII Intel HEX string to HEXstring_struct
         * Discards first ":" symbol
         */
        parseIntelHEXstr(&g_HexStringStruct_buffer, g_lineBuffer_ptr + 1);

        /*
         * Perform crypto function if data firmware exists
         * Expand data field up to 16 bytes (2*DES_BLOCK_SIZE) by adding '0xFF' (empty NAND flash byte) to tail
         */
        if (g_HexStringStruct_buffer.type == HEX_DATA) {                    //expand data if data length isn't max
            if (g_HexStringStruct_buffer.data_length < 2 * DES_BLOCK_SIZE) {
                for (size_t i = g_HexStringStruct_buffer.data_length;
                     i < 2 * DES_BLOCK_SIZE; i++)
                    g_HexStringStruct_buffer.data[i] = EMPTY_FLASH_BYTE;    //fill by 0xFF up to max length
                g_HexStringStruct_buffer.data_length = 2 * DES_BLOCK_SIZE;  //update length field
            }
            crypto_function(
                    g_HexStringStruct_buffer.data);                         //perform crypto on first 8 bytes of HEX string
            crypto_function(
                    g_HexStringStruct_buffer.data +
                    DES_BLOCK_SIZE);                                        //perform crypto on next 8 bytes of HEX string
        }

        updateCRCfield(&g_HexStringStruct_buffer);                          //recalculate CRC field

        /*
         * Serialize HEXstring_struct to Intel HEX ASCII format line
         * Write resulting string to result file
         */
        fputs(serializeHexStruct(&g_HexStringStruct_buffer), g_EncryptedFile_ptr);

        //clear HEXstring_struct buffer
        hexStringStruct_clearAllFields(&g_HexStringStruct_buffer);
    }

    /*
     * Free resources,
     * close all opened files
     */
    fclose(g_PlainFile_ptr);
    fclose(g_EncryptedFile_ptr);

    //print disclaimer
    printf("firmware encryptor ends, all done\r\n");
    return 0;
}

//region crypto function implementations
/**************************** FUNCTIONS IMPLEMENTATION *****************************/

void tDesPerformOn(uint8_t *plainData) {
    uint8_t tDesSchedule_buffer[3][16][6];
    uint8_t encryptedDataBuffer_ptr[DES_BLOCK_SIZE];

    //set crypto mode
    CryptoMode_enum crypto_mode = ((g_Des_mode == TDES_ENCRYPTION) ? ENCRYPT : DECRYPT);
    //setup buffer for keys
    three_des_key_setup(g_DES_KEY_ptr, tDesSchedule_buffer, crypto_mode);
    //apply to data
    three_des_crypt(plainData, encryptedDataBuffer_ptr, tDesSchedule_buffer);

    //replace plain data by encrypted data
    memcpy(plainData, encryptedDataBuffer_ptr, DES_BLOCK_SIZE);

    //debug print
    //for (int i=0; i<DES_BLOCK_SIZE;i++) printf("%02X ",plainData[i]);
}

void desPerformOn(uint8_t *plainData) {
    uint8_t desSchedule_buffer[16][6];
    uint8_t encryptedDataBuffer_ptr[DES_BLOCK_SIZE];

    //set crypto mode
    CryptoMode_enum crypto_mode = ((g_Des_mode == DES_ENCRYPTION) ? ENCRYPT : DECRYPT);
    //setup buffer for keys
    des_key_setup(g_DES_KEY_ptr, desSchedule_buffer, crypto_mode);
    //apply to data
    des_crypt(plainData, encryptedDataBuffer_ptr, desSchedule_buffer);

    //replace plain data by encrypted data
    memcpy(plainData, encryptedDataBuffer_ptr, DES_BLOCK_SIZE);

    //debug print
    //for (int i=0; i<DES_BLOCK_SIZE;i++) printf("%02X ",plainData[i]);
}
//endregion






