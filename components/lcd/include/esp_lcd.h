#pragma once
#include "esp_lcd_phy.h"
#include "esp_err.h"
#include "freertos/semphr.h"

#define LCD_CHECK(a, str, goto_tag,  ret_value, ...)                              \
    do                                                                            \
    {                                                                             \
        if (!(a))                                                                 \
        {                                                                         \
            ESP_LOGE(TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            ret = ret_value;                                                      \
            goto goto_tag;                                                        \
        }                                                                         \
    } while (0)

/**
* @brief Type of Lcd deviece
*
*/
typedef enum  {
    LCD_TYPE_ILI9341 = 0,
    LCD_TYPE_ST7789
} lcd_type_t;

/**
* @brief Handle of lcd;
*
*/
typedef struct esp_lcd_handle_s esp_lcd_handle_t;
struct esp_lcd_handle_s {

    /**
    * @brief Type of lcd screen
    *
    */
    lcd_type_t lcd_type;

    /**
    * @brief Width of screen
    *
    */
    int width;

    /**
    * @brief Height of screen
    *
    */
    int height;

    /**
    * @brief Semaphore for lcd operation
    *
    */
    SemaphoreHandle_t mux;

    /**
    * @brief abstract physical interface for write data or command
    * phy should privide write_data write command
    *
    */
    esp_lcd_phy_t *phy;

    /**
    * @brief Send commands to init lcd deviece
    *
    * @param[in]  hdl: lcd handle
    *
    * @return
    *       - ESP_OK: send data successfully
    *       - ESP_FAIL: error occur in transmit data
    *       - ESP_ERR_INVALID_ARG: phy is not init
    */
    esp_err_t (*start)(esp_lcd_handle_t *hdl);

    /**
    * @brief Send data to display
    *
    * @param[in]  hdl: handle of lcd
    * @param[in]  data: pointer of data
    * @param[in]  info: size and type of data to transmit
    * @return
    *       - ESP_OK: send data successfully
    *       - ESP_FAIL: error occur in transmit data
    */
    esp_err_t (*set_frame)(esp_lcd_handle_t *hdl, void *data, data_info_t info);

    /**
    * @brief Send commands to init lcd deviece
    *
    * @param[in]  hdl: handle of lcd
    * @param[in]  data: pointer of data
    * @param[in]  info: size and type of data to transmit
    * @return
    *       - ESP_OK: send data successfully
    *       - ESP_FAIL: error occur in transmit data
    */
    esp_err_t (*set_window)(esp_lcd_handle_t *hdl, int x_start, int x_end, int y_start, int y_end);

    /**
    * @brief delet lcd deviece
    *
    * @param[in]  hdl: lcd handle
    *
    * @return
    *       - ESP_OK: send data successfully
    *       - ESP_FAIL: error occur in transmit data
    */
    esp_err_t (*delete)(esp_lcd_handle_t *hdl);

};

/**
* @brief Create LCD handle fatory
*
* @param[in]  type: type of lcd
* @param[in]  phy: abstuct handle of physical interface
* @param[in]  width: width of lcd
* @param[in]  height: height of lcd
*
* @return
*       - instance: create LCD instance successfully
*       - NULL: create LCD instance failed because some error occurred
*/
esp_lcd_handle_t *esp_lcd_factory(lcd_type_t type, esp_lcd_phy_t *phy, int width, int height);


/**
*  lcd intance , implements functions in esp_lcd_handle_t
*/

esp_err_t st7789v_start(esp_lcd_handle_t *hdl);

esp_err_t st7789v_set_frame(esp_lcd_handle_t *hdl, void *data, data_info_t info);

esp_err_t st7789v_set_window(esp_lcd_handle_t *hdl, int x_start, int x_end, int y_start, int y_end);

esp_err_t st7789v_delete(esp_lcd_handle_t *hdl);
