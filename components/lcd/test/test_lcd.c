#include "test_utils.h"
#include "unity.h"
#include <sys/time.h>
#include "esp_lcd.h"
#include "esp_log.h"
#include "lcd_image.h"
#include "freertos/queue.h"

static const char *TAG = "lcd_test";

u_int16_t *buf_dram;
u_int16_t *buf_psram;


void app_lcd_task()
{
    ESP_LOGI(TAG, "Enter task");

    uint8_t i = 0;
    uint32_t time, t1, t2 = 0;

    data_info_t info = {
        .size = 320 * 240,
        .type = DATA_TYPE_UINT16
    };
    lcd_phy_spi_config_t conf;

    /* create handles, type is ST7789 */
    esp_lcd_phy_t *phy =  esp_lcd_phy_spi_factory();
    esp_lcd_handle_t *lcd = esp_lcd_factory(LCD_TYPE_ST7789, phy, 320, 240);

    /* init spi phy interface*/
    load_phy_spi_config_default(&conf);
    lcd->phy->init(lcd->phy, (void *)&conf);

    /* inti ST7789 */
    lcd->start(lcd);

    /* set frame size */
    lcd->set_window(lcd,  0, 320 - 1, 0, 240 - 1);

    ESP_LOGI(TAG, "\n\n-------------------refresh in dram-------------------");
    time = xTaskGetTickCount();
    while (true) {
        if ((xTaskGetTickCount() - time) > 1000 / portTICK_RATE_MS ) {
            ESP_LOGI(TAG, "result: lcd refresh %d fps", i);
            time = xTaskGetTickCount();
            i = 0;
            break;
        }
        i++;

        t1 = xTaskGetTickCount();
        lcd->set_frame(lcd, (void *)buf_dram, info);
        t2 = xTaskGetTickCount();

        ESP_LOGI(TAG, "frame %d cost time : %d ms", i, (t2 - t1));
    }

    free(buf_dram);
    vTaskDelay(1500 / portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "\n\n-------------------refresh in psram-------------------");
    time = xTaskGetTickCount();
    while (true) {
        if ((xTaskGetTickCount() - time) > 1000 / portTICK_RATE_MS ) {
            ESP_LOGI(TAG, "result: lcd refresh %d fps", i);
            time = xTaskGetTickCount();
            i = 0;
            break;
        }
        i++;

        t1 = xTaskGetTickCount();
        lcd->set_frame(lcd, (void *)buf_psram, info);
        t2 = xTaskGetTickCount();

        ESP_LOGI(TAG, "frame %d cost time : %d ms", i, (t2 - t1));
    }
    free(buf_psram);
    vTaskDelay(1500 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "\n\n-------------------refresh const data-------------------");
    time = xTaskGetTickCount();
    while (true) {
        if ((xTaskGetTickCount() - time) > 1000 / portTICK_RATE_MS ) {
            ESP_LOGI(TAG, "result: lcd refresh %d fps", i);
            time = xTaskGetTickCount();
            i = 0;
            break;
        }
        i++;

        t1 = xTaskGetTickCount();
        lcd->set_frame(lcd, (void *)pic1_320_240, info);
        t2 = xTaskGetTickCount();

        ESP_LOGI(TAG, "frame %d cost time : %d ms", i, (t2 - t1));
    }
    vTaskDelete(NULL);
}

TEST_CASE("lcd test", "[test][leaks=0]")
{
    /* init buffer in dram*/
    buf_dram = (uint16_t *)heap_caps_malloc(sizeof(uint16_t) * (320 * 240), MALLOC_CAP_INTERNAL);
    for (int i = 0; i < 320 * 240 - 1; i++) {
        buf_dram[i] = 0xfdf3;
    }

    /* init buffer in psram*/
    buf_psram = (uint16_t *)heap_caps_malloc(sizeof(uint16_t) * (320 * 240), MALLOC_CAP_SPIRAM);
    for (int i = 0; i < 320 * 240 - 1; i++) {
        buf_psram[i] = 0x2df3;
    }

    ESP_LOGD(TAG, "Starting app_lcd_task...");

    /* Create test task*/
    xTaskCreate(&app_lcd_task, "app_lcd_task", 4096 * 2, NULL, 4, NULL);

    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
