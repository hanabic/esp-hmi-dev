#include "esp_lcd_phy.h"
#include "esp_err.h"
#include "esp_lcd_common.h"
#include "freertos/semphr.h"

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
typedef struct esp_lcd_handle_s {

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

} esp_lcd_handle_t;


esp_lcd_handle_t *esp_lcd_factory(lcd_type_t type, esp_lcd_phy_t *phy, int width, int height);

esp_err_t esp_lcd_start(esp_lcd_handle_t *lcd_handle);

esp_err_t esp_lcd_set_frame(esp_lcd_handle_t *lcd_handle, esp_lcd_data_t *frame);

esp_err_t esp_lcd_set_window(esp_lcd_handle_t *lcd_handle, int x_start, int y_start, int x_end, int y_end, esp_lcd_data_t *window);

esp_err_t esp_lcd_delete(esp_lcd_handle_t *lcd_handle);


