//
// Created by Max on 16.02.2016.
//
#include <stdlib.h>
#include "hex_string.h"

bool parseIntelHEXstr(HEXstring_struct *hexStringStruct_ptr, char *data_ptr) {
    char value_buffer[sizeof(uint32_t) + 1];//5 bytes long buffer

    //get length
    strncpy(value_buffer, data_ptr, HEX_STR_LENGTH_LENGTH_BYTES);
    value_buffer[HEX_STR_LENGTH_LENGTH_BYTES] = '\0';
    hexStringStruct_ptr->data_length = (size_t) strtol(value_buffer, NULL, 16);

    //get address
    strncpy(value_buffer, data_ptr + HEX_STR_LENGTH_LENGTH_BYTES, HEX_STR_ADDR_LENGTH_BYTES);
    value_buffer[HEX_STR_ADDR_LENGTH_BYTES] = '\0';
    hexStringStruct_ptr->address = (uint16_t) strtol(value_buffer, NULL, 16);

    //get type
    strncpy(value_buffer, data_ptr + HEX_STR_LENGTH_LENGTH_BYTES + HEX_STR_ADDR_LENGTH_BYTES,
            HEX_STR_TYPE_LENGTH_BYTES);
    value_buffer[HEX_STR_TYPE_LENGTH_BYTES] = '\0';
    hexStringStruct_ptr->type = (HEXstring_types_enum) strtol(value_buffer, NULL, 16);

    //get data
    value_buffer[ASCII_BYTE_LENGTH] = '\0';
    for (int i = 0; i < hexStringStruct_ptr->data_length; i++) {
        strncpy(value_buffer,
                data_ptr + HEX_STR_LENGTH_LENGTH_BYTES + HEX_STR_ADDR_LENGTH_BYTES + HEX_STR_TYPE_LENGTH_BYTES +
                (ASCII_BYTE_LENGTH * i), ASCII_BYTE_LENGTH);
        hexStringStruct_ptr->data[i] = (uint8_t) strtol(value_buffer, NULL, 16);
    }

    //get crc
    strncpy(value_buffer,
            data_ptr + HEX_STR_LENGTH_LENGTH_BYTES + HEX_STR_ADDR_LENGTH_BYTES + HEX_STR_TYPE_LENGTH_BYTES +
            (ASCII_BYTE_LENGTH * hexStringStruct_ptr->data_length), HEX_STR_CRC_LENGTH_BYTES);
    value_buffer[HEX_STR_CRC_LENGTH_BYTES] = '\0';
    hexStringStruct_ptr->crc = (uint8_t) strtol(value_buffer, NULL, 16);

    return true;
}

uint8_t *serializeHexStruct(HEXstring_struct *hexStringStruct_ptr) {
    static uint8_t asciiHexString_buffer[HEX_LINE_ASCII_MAX_LENGTH + 1];

    //clear buffer
    memset(asciiHexString_buffer, '\0', HEX_LINE_ASCII_MAX_LENGTH);

    //serialize start ":"
    asciiHexString_buffer[HEX_STR_START_BYTE_LENGTH - 1] = HEX_STR_START_BYTE;

    //serialize length + address + type
    sprintf(asciiHexString_buffer + HEX_STR_START_BYTE_LENGTH, "%02X%04X%02X", hexStringStruct_ptr->data_length,
            hexStringStruct_ptr->address, hexStringStruct_ptr->type);

    //serialize data according to length
    for (int hexStructData_iterator = 0;
         hexStructData_iterator < hexStringStruct_ptr->data_length; hexStructData_iterator++) {
        sprintf(asciiHexString_buffer + 2 * hexStructData_iterator + \
        HEX_STR_START_BYTE_LENGTH + HEX_STR_LENGTH_LENGTH_BYTES + HEX_STR_ADDR_LENGTH_BYTES + HEX_STR_TYPE_LENGTH_BYTES, \
        "%02X", hexStringStruct_ptr->data[hexStructData_iterator]);
    }

    //serialize control sum and last "\r"
    sprintf(asciiHexString_buffer + 2 * hexStringStruct_ptr->data_length + \
        HEX_STR_START_BYTE_LENGTH + HEX_STR_LENGTH_LENGTH_BYTES + HEX_STR_ADDR_LENGTH_BYTES + HEX_STR_TYPE_LENGTH_BYTES, \
        "%02X\r", hexStringStruct_ptr->crc);

    //debug print
    /*printf("max len: %d, this len:%d, ASCII: %s\r\n", HEX_LINE_ASCII_MAX_LENGTH, strlen(asciiHexString_buffer),
           asciiHexString_buffer);*/

    return asciiHexString_buffer;
}

void updateCRCfield(HEXstring_struct *hexStringStruct_ptr) {
    uint16_t newCRC = 0;

    //sum LL and TT field by mod 256
    newCRC = (newCRC + hexStringStruct_ptr->data_length) % 0x100;
    newCRC = (newCRC + hexStringStruct_ptr->type) % 0x100;
    newCRC = (newCRC + ((hexStringStruct_ptr->address) & 0x00FF)) % 0x100;
    newCRC = (newCRC + ((hexStringStruct_ptr->address) >> 8)) % 0x100;

    //add sum data by mod 256
    for (int i = 0; i < hexStringStruct_ptr->data_length; i++) newCRC = (newCRC + hexStringStruct_ptr->data[i]) % 0x100;

    //finally
    newCRC = (uint8_t) (0 - newCRC);

    //update HEXstring_struct
    hexStringStruct_ptr->crc = newCRC;
}

void hexStringStruct_clearAllFields(HEXstring_struct *hexStringStruct_ptr) {
    memset(&hexStringStruct_ptr->address, '\0', sizeof(hexStringStruct_ptr->address));
    memset(&hexStringStruct_ptr->crc, '\0', sizeof(hexStringStruct_ptr->crc));
    memset(&hexStringStruct_ptr->data, '\0', sizeof(hexStringStruct_ptr->data));
    memset(&hexStringStruct_ptr->data_length, '\0', sizeof(hexStringStruct_ptr->data_length));
    memset(&hexStringStruct_ptr->type, '\0', sizeof(hexStringStruct_ptr->type));
}

//for debug purposes
void hexStringStruct_print(HEXstring_struct *hexStringStruct_ptr) {
    printf("len : %02X addr: %04X type: %02X ", hexStringStruct_ptr->data_length, hexStringStruct_ptr->address,
           (uint16_t) hexStringStruct_ptr->type);
    printf("data: ");
    for (int i = 0; i < sizeof(hexStringStruct_ptr->data); i++) printf("0x%02X ", hexStringStruct_ptr->data[i]);
    printf(" crc : %02X\r\n", hexStringStruct_ptr->crc);
}

