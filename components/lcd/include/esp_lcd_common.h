#include <stdint.h>

/**
* @brief Type of lcd buffer: uint8, uint 16, uint 32
*
*/

typedef enum {
    BUF_TYPE_UINT8 = 0,
    BUF_TYPE_UINT16,
    BUF_TYPE_UINT32
} data_type_t;

typedef union {
    uint8_t *buf_uint8;
    uint16_t *buf_uint16;
    uint32_t *buf_uint32;
} data_ptr_t;

/**
* @brief Define lcd buffer, support different type of data;
*
*/
typedef struct esp_lcd_data_t {
    data_type_t type;
    data_ptr_t buf_ptr;
    int size;
};