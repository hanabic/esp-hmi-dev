#include "esp_lcd.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lcd_init_cmd.h"

esp_lcd_handle_t *esp_lcd_factory(lcd_type_t type, esp_lcd_phy_t *phy, int width, int height)
{
    esp_lcd_handle_t *lcd_handle = (esp_lcd_handle_t *)calloc(1, sizeof(esp_lcd_handle_t));
    lcd_handle->lcd_type = type;
    lcd_handle->width = width;
    lcd_handle->height = height;
    lcd_handle->mux = xSemaphoreCreateBinary();
    lcd_handle->phy = phy;

    switch (type) {

    case LCD_TYPE_ST7789: {
        lcd_handle->start = st7789v_start;
        lcd_handle->delete = st7789v_delete;
        lcd_handle->set_frame = st7789v_set_frame;
        lcd_handle->set_window = st7789v_set_window;
        break;
    }

    case LCD_TYPE_ILI9341: {
        break;
    }

    }
    return lcd_handle;
}

