//
// Created by Max on 16.02.2016.
//

#ifndef ENCRYPTORTEST_HEXPARSER_H
#define ENCRYPTORTEST_HEXPARSER_H

#include "system.h"
/*
*Intel HEX format definition
*/
#define HEX_STR_START_BYTE_LENGTH       1
#define ASCII_BYTE_LENGTH				2

#define HEX_STR_ASCII_MAX_LENGTH		32
#define HEX_LINE_ASCII_MAX_LENGTH		46
#define HEX_STR_DATA_MAX_LENGTH_BYTES	16

#define HEX_STR_LENGTH_LENGTH_BYTES		2
#define HEX_STR_ADDR_LENGTH_BYTES		4
#define HEX_STR_TYPE_LENGTH_BYTES		2
#define HEX_STR_CRC_LENGTH_BYTES		2

#define HEX_STR_START_BYTE			    ':'

/*
*Intel HEX string types
*/
typedef enum{
    HEX_DATA,
    HEX_END_OF_FIRMWARE_DATA,
    HEX_SEGMENT_ADDRESS,
    HEX_EXPANDED_ADDRESS
}HEXstring_types_enum;

/*
*parsed Intel HEX structure
*/
typedef struct{
    size_t					data_length;
    uint16_t				address;
    HEXstring_types_enum	type;
    uint8_t					data[HEX_STR_ASCII_MAX_LENGTH / 2];
    uint8_t					crc;
}HEXstring_struct;

/*
 * Parse ASCII Intel HEX string to hexStringStruct_ptr which is more convenient to work with.
 */
bool parseIntelHEXstr(HEXstring_struct* hexStringStruct_ptr,char* data_ptr);

/*
 * Serialize HEXstring_struct to ascii Intel HEX
 */
uint8_t *serializeHexStruct(HEXstring_struct* hexStringStruct_ptr);

/*
 * Calculate uint8_t crc field and put it back to struct
 */
void updateCRCfield(HEXstring_struct* hexStringStruct_ptr);

/*
 * Clear all field of HEXstring_struct
 */
void hexStringStruct_clearAllFields(HEXstring_struct *hexStringStruct_ptr);

/*
* Debug pretty print to stdout
*/
void hexStringStruct_print(HEXstring_struct *hexStringStruct_ptr);

#endif //ENCRYPTORTEST_HEXPARSER_H
