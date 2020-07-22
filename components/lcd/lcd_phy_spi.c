#include "string.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_lcd_phy.h"

static const char *TAG = "spi_phy";

#define LCD_HOST    HSPI_HOST
#define DMA_CHAN    2

#define PIN_NUM_MISO 25
#define PIN_NUM_MOSI 9
#define PIN_NUM_CLK  15
#define PIN_NUM_CS   11

#define PIN_NUM_DC   13
#define PIN_NUM_RST  16
#define PIN_NUM_BCKL 6

#define PHY_CHECK(a, str, goto_tag, ret_value, ...)                               \
    do                                                                            \
    {                                                                             \
        if (!(a))                                                                 \
        {                                                                         \
            ESP_LOGE(TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            ret = ret_value;                                                      \
            goto goto_tag;                                                        \
        }                                                                         \
    } while (0)

esp_err_t esp_lcd_phy_spi_init(esp_lcd_phy_t *phy, void *cfg)
{
    esp_err_t ret = ESP_OK;

    /* Get spi driver handle */
    lcd_phy_spi_t *phy_spi = __containerof(phy, lcd_phy_spi_t, parent);
    PHY_CHECK(phy_spi, "phy_spi is null", err, ESP_ERR_INVALID_ARG);
    PHY_CHECK(!phy->isInited, "spi has been inited", err, ESP_FAIL);

    /* Confirm configs in setting */
    lcd_phy_spi_config_t *spi_cfg = (lcd_phy_spi_config_t *)cfg;
    PHY_CHECK(spi_cfg, "can't set phy config to null", err, ESP_ERR_INVALID_ARG);
    phy_spi->host = spi_cfg->host;

    /* Init spi bus */
    ret = spi_bus_initialize(spi_cfg->host, &spi_cfg->buscfg, spi_cfg->dma_chan);
    PHY_CHECK(ret == ESP_OK, "init spi bus failed", err, ESP_FAIL);

    /* Add device to spi bus */
    ret = spi_bus_add_device(spi_cfg->host, &spi_cfg->devcfg, &phy_spi->spi);
    PHY_CHECK(ret == ESP_OK, "add device to spi bus failed", err, ESP_FAIL);

    phy->isInited = true;
    phy->dc = spi_cfg->dc;
    phy->rst = spi_cfg->rst;
    phy->blk = spi_cfg->blk;

    return ESP_OK;
err:
    return ret;
}

esp_err_t esp_lcd_phy_spi_deinit(esp_lcd_phy_t *phy)
{
    esp_err_t ret = ESP_OK;

    /* Get spi driver handle */
    lcd_phy_spi_t *phy_spi = __containerof(phy, lcd_phy_spi_t, parent);
    PHY_CHECK(phy_spi, "phy_spi is null", err, ESP_FAIL);
    PHY_CHECK(phy->isInited, "spi hasn't been inited", err, ESP_FAIL);

    /* Remove devices form spi bus and free bus */
    PHY_CHECK(spi_bus_remove_device(phy_spi->spi) == ESP_OK, "remove device from spi bus failed", err, ESP_FAIL);
    PHY_CHECK(spi_bus_free(phy_spi->host) == ESP_OK, "free bus failed", err, ESP_FAIL);
    memset(phy, 0, sizeof(esp_lcd_phy_t));

    return ESP_OK;

err:
    return ret;
}

esp_err_t esp_lcd_phy_spi_write_cmd(esp_lcd_phy_t *phy, uint8_t cmd)
{
    esp_err_t ret = ESP_OK;

    /* Get spi driver handle */
    lcd_phy_spi_t *phy_spi = __containerof(phy, lcd_phy_spi_t, parent);
    PHY_CHECK(phy_spi, "phy_spi is null", err, ESP_ERR_INVALID_ARG);

    /* Build sppi transmiton struct */
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.rxlength = 0;
    t.tx_buffer = &cmd;
    t.user = (void *)0;
    t.flags = 0;

    /* For faster speed, transalte by polling */
    PHY_CHECK(spi_device_polling_transmit(phy_spi->spi, &t) == ESP_OK, "send cmd failed", err, ESP_FAIL);

    return ESP_OK;

err:
    return ret;
}

// static spi_transaction_t td;
esp_err_t esp_lcd_phy_spi_write_data(esp_lcd_phy_t *phy, void *data, data_info_t info)
{
    esp_err_t ret = ESP_OK;

    /* Get spi driver handle */
    spi_transaction_t td;
    lcd_phy_spi_t *phy_spi = __containerof(phy, lcd_phy_spi_t, parent);
    PHY_CHECK(phy_spi, "phy_spi is null", err, ESP_ERR_INVALID_ARG);

    /* Build sppi transmiton struct. dectect data length by configed in data */
    td.length = info.type * info.size;
    td.tx_buffer = (void *)data;
    td.user = (void *)1;
    td.flags = 0; /*undo flags*/
    td.rx_buffer = NULL;
    td.rxlength = 0;

    /* For faster speed, transalte by polling */
    PHY_CHECK(spi_device_polling_transmit(phy_spi->spi, &td) == ESP_OK, "send data failed", err, ESP_FAIL); /*transmited!*/
    return ESP_OK;

err:
    return ret;
}

esp_lcd_phy_t *esp_lcd_phy_spi_factory()
{
    esp_err_t ret;

    lcd_phy_spi_t *phy_spi = calloc(1, sizeof(lcd_phy_spi_t));
    PHY_CHECK(phy_spi, "calloc phy_spi failed", err, NULL);

    phy_spi->parent.isInited = false;
    phy_spi->parent.init = esp_lcd_phy_spi_init;
    phy_spi->parent.deinit = esp_lcd_phy_spi_deinit;
    phy_spi->parent.write_cmd = esp_lcd_phy_spi_write_cmd;
    phy_spi->parent.write_data = esp_lcd_phy_spi_write_data;

    return &(phy_spi->parent);

err:
    return NULL;
}

void lcd_spi_pre_transfer_callback(spi_transaction_t *t)
{
    int dc = (int)t->user;
    gpio_set_level(PIN_NUM_DC, dc);
}

void load_phy_spi_config_default(lcd_phy_spi_config_t *config)
{
    config->buscfg.mosi_io_num = PIN_NUM_MOSI;
    config->buscfg.sclk_io_num = PIN_NUM_CLK;
    config->buscfg.quadwp_io_num = -1;
    config->buscfg.quadhd_io_num = -1;
    config->buscfg.max_transfer_sz = 320 * 240 * 16 * 2;

    config->devcfg.clock_speed_hz = SPI_MASTER_FREQ_40M;
    config->devcfg.mode = 0;
    config->devcfg.spics_io_num = PIN_NUM_CS;
    config->devcfg.queue_size = 200;
    config->devcfg.pre_cb = lcd_spi_pre_transfer_callback;


    config->blk = PIN_NUM_BCKL;
    config->dc = PIN_NUM_DC;
    config->rst = PIN_NUM_RST;
    config->dma_chan = DMA_CHAN;
    config->host = LCD_HOST;
}