#include "esp_lcd.h"
#include "driver/gpio.h"
#include "lcd_init_cmd.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "st7789v lcd";

esp_err_t st7789v_start(esp_lcd_handle_t *hdl)
{
    const lcd_init_cmd_t *lcd_init_cmds = st_init_cmds;
    esp_err_t ret;
    int cmd = 0;

    gpio_set_direction(hdl->phy->dc, GPIO_MODE_OUTPUT);
    gpio_set_direction(hdl->phy->rst, GPIO_MODE_OUTPUT);
    gpio_set_direction(hdl->phy->blk, GPIO_MODE_OUTPUT);

    //Reset the display
    gpio_set_level(hdl->phy->rst, 0);
    vTaskDelay(100 / portTICK_RATE_MS);
    gpio_set_level(hdl->phy->rst, 1);
    vTaskDelay(100 / portTICK_RATE_MS);


    while (lcd_init_cmds[cmd].databytes != 0xff) {
        data_info_t info = {
            .size = lcd_init_cmds[cmd].databytes & 0x1F,
            .type = DATA_TYPE_UINT8
        };

        LCD_CHECK(hdl->phy->write_cmd(hdl->phy, lcd_init_cmds[cmd].cmd) == ESP_OK, "write init cmd fail", err, ESP_FAIL);

        if (lcd_init_cmds[cmd].databytes & 0x80) {
            vTaskDelay(100 / portTICK_RATE_MS);
        } else {
            LCD_CHECK(hdl->phy->write_data(hdl->phy, (void *)lcd_init_cmds[cmd].data, info) == ESP_OK, "write init data fail", err, ESP_FAIL);
        }
        cmd++;
    }
    ///Enable backlight
    gpio_set_level(hdl->phy->blk, 0);

    return ESP_OK;
err:
    return ret;
}

esp_err_t st7789v_set_frame(esp_lcd_handle_t *hdl, void *data, data_info_t info)
{
    esp_err_t ret;

    LCD_CHECK(hdl->phy->write_cmd(hdl->phy, 0x2c) == ESP_OK, "set frame cmd:0x2c failed", err, ESP_FAIL);
    LCD_CHECK(hdl->phy->write_data(hdl->phy, data, info) == ESP_OK, "set frame data failed", err, ESP_FAIL);

    return ESP_OK;

err:
    return ret;
}

esp_err_t st7789v_set_window(esp_lcd_handle_t *hdl, int x_start, int x_end, int y_start, int y_end)
{
    esp_err_t ret;
    data_info_t info_cmd = {
        .size = 4,
        .type = DATA_TYPE_UINT8
    };

    uint8_t cmd_xdatas[4] = {x_start, x_start & 0xFF, x_end >> 8, x_end & 0xFF};
    uint8_t cmd_ydatas[4] = {y_start, y_start & 0xFF, y_end >> 8, y_end & 0xFF};

    LCD_CHECK(hdl->phy->write_cmd(hdl->phy, 0x2a) == ESP_OK, "set window cmd:0x2a failed", err, ESP_FAIL);
    LCD_CHECK(hdl->phy->write_data(hdl->phy, (void *)cmd_xdatas, info_cmd) == ESP_OK, "set window data failed", err, ESP_FAIL);

    LCD_CHECK(hdl->phy->write_cmd(hdl->phy, 0x2b) == ESP_OK, "set window cmd:0x2b failed", err, ESP_FAIL);
    LCD_CHECK(hdl->phy->write_data(hdl->phy, (void *)cmd_ydatas, info_cmd) == ESP_OK, "set window data failed", err, ESP_FAIL);

    return ESP_OK;

err:
    return ret;
}

esp_err_t st7789v_delete(esp_lcd_handle_t *hdl)
{
    return ESP_OK;
}