#pragma once
#include "esp_err.h"
#include "esp_lcd_common.h"
#include "driver/spi_master.h"

typedef enum {
    LCD_PHY_TYPE_SPI = 0,
    LCD_PHY_TYPE_I2S = 1,
    LCD_PHY_TYPE_RGB8 = 2,
} esp_lcd_phy_type_t;

/**
* @brief lcd model ,driver will implement methods.
*
*/
typedef struct esp_lcd_phy_s esp_lcd_phy_t;
struct esp_lcd_phy_s {
    /**
    * @brief control data/command pin
    *
    */
    int dc;

    /**
    * @brief reset pin
    *
    */
    int rst;

    /**
    * @brief backlight pin
    *
    */
    int blk;

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
    esp_err_t(*init)(esp_lcd_phy_t *phy, void *conf);

    /**
    * @brief The spi handle.
    *
    */
    bool isInited;

    /**
    * @brief Write command interface, will set by different driver
    *
    */
    esp_err_t(*write_cmd)(esp_lcd_phy_t *phy, uint8_t cmd);

    /**
    * @brief Write data interface, will set by different driver
    *
    */
    esp_err_t(*write_data)(esp_lcd_phy_t *phy, void *data, data_info_t info);

    /**
    * @brief Physical driver deinit
    *
    */
    esp_err_t(*deinit)(esp_lcd_phy_t *phy);

};

/**
* @brief The spi driver model.
*
*/
typedef struct lcd_phy_spi_s {
    /**
    * @brief The lcd_phy_t address will be return after driver init.
    *        SPI drivers get handle address by parents.
    *
    */
    esp_lcd_phy_t parent;

    /**
    * @brief The spi handle.
    *
    */
    spi_device_handle_t spi;

    /**
    * @brief Host id of spi.
    *
    */
    spi_host_device_t host;

} lcd_phy_spi_t;

/**
* @brief Config struct for spi driver.
*
*/
typedef struct {
    /**
    * @brief spi bus config
    *
    */
    spi_bus_config_t buscfg;

    /**
    * @brief spi device config
    *
    */
    spi_device_interface_config_t devcfg;

    /**
    * @brief host of spi HSPI or VSPI
    *
    */
    spi_host_device_t host;

    /**
    * @brief control data/command pin
    *
    */
    int dma_chan;

    /**
    * @brief control data/command pin
    *
    */
    int dc;

    /**
    * @brief reset pin
    *
    */
    int rst;

    /**
    * @brief backlight pin
    *
    */
    int blk;

} lcd_phy_spi_config_t;

/**
* @brief Create LCD phy instace fatory
*
* @return
*       - instance: create phy lcd instance successfully
*       - NULL: create lcd phy instance failed because some error occurred
*/
esp_lcd_phy_t *esp_lcd_phy_spi_factory();

/**
* @brief Load default config for spi
*
* @param[out]  config
*
*/
void load_phy_spi_config_default(lcd_phy_spi_config_t *config);