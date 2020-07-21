#include <stdint.h>

/**
* @brief Type of lcd buffer: uint8, uint 16, uint 32
*
*/

typedef enum {
    DATA_TYPE_UINT8 = 8,
    DATA_TYPE_UINT16 = 16,
    DATA_TYPE_UINT32 = 32
} data_type_t;

/**
* @brief data info struct;
*
*/
typedef struct {
    data_type_t type;
    int size;
} data_info_t;