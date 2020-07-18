#include "esp_err.h"
#include "esp_lcd_common.h"

typedef enum {
    LCD_PHY_TYPE_SPI = 0,
    LCD_PHY_TYPE_I2S = 1,
    LCD_PHY_TYPE_RGB8 = 2,
} esp_lcd_phy_type_t;



typedef struct esp_lcd_phy_s {
    /**
    * @brief Type of lcd driver
    *
    */
    esp_lcd_phy_type_t type;

    /**
    * @brief Data type: uint8, uint16, uint32
    *
    */
    data_type_t base_data_type;

    /**
    * @brief Physical driver init
    *
    */
    esp_err_t(*phy_init)();

    /**
    * @brief Write command interface, will set by different driver
    *
    */
    esp_err_t(*write_cmd)(esp_lcd_data_t cmd);

    /**
    * @brief Write data interface, will set by different driver
    *
    */
    esp_err_t(*write_data)(esp_lcd_data_t data);

    /**
    * @brief Physical driver deinit
    *
    */
    esp_err_t(*phy_deinit)();

} esp_lcd_phy_t;


typedef struct lcd_phy_spi_s {
    /**
    * @brief base lcd_phy_t address will be return after driver init
    *
    */
    esp_lcd_phy_t parent;

    //TODO: spi struct functions and members
} lcd_phy_spi_t;

esp_lcd_phy_t *esp_lcd_phy_spi_factory();