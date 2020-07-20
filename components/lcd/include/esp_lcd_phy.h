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
    esp_err_t(*phy_init)(esp_lcd_phy_t *phy, void *conf);

    /**
    * @brief Write command interface, will set by different driver
    *
    */
    esp_err_t(*write_cmd)(esp_lcd_phy_t *phy, uint8_t cmd);

    /**
    * @brief Write data interface, will set by different driver
    *
    */
    esp_err_t(*write_data)(esp_lcd_phy_t *phy, esp_lcd_data_t data);

    /**
    * @brief Physical driver deinit
    *
    */
    esp_err_t(*phy_deinit)(esp_lcd_phy_t *phy);

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
    * @brief The spi handle.
    *
    */
    bool isInited;

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

    spi_bus_config_t buscfg;

    spi_device_interface_config_t devcfg;

    spi_host_device_t host;

    int dma_chan;

} lcd_phy_spi_config_t;

esp_lcd_phy_t *esp_lcd_phy_spi_factory(const lcd_phy_spi_config_t *cfg);