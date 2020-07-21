#include "string.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_lcd_phy.h"

static const char *TAG = "spi_phy";

#define PHY_CHECK(a, str, goto_tag, ...)                                          \
    do                                                                            \
    {                                                                             \
        if (!(a))                                                                 \
        {                                                                         \
            ESP_LOGE(TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            goto goto_tag;                                                        \
        }                                                                         \
    } while (0)

static esp_err_t esp_lcd_phy_spi_init(esp_lcd_phy_t *phy, void *cfg)
{
    esp_err_t ret;

    /* Get spi driver handle */
    lcd_phy_spi_t *phy_spi = __containerof(phy, lcd_phy_spi_t, parent);
    PHY_CHECK(phy_spi, "phy_spi is null", err);

    PHY_CHECK(!phy->isInited, "spi has been inited", err);

    /* Confirm configs in setting */
    lcd_phy_spi_config_t *spi_cfg = (lcd_phy_spi_config_t *)cfg;
    PHY_CHECK(spi_cfg, "can't set phy config to null", err);
    phy_spi->host = spi_cfg->host;

    /* Init spi bus */
    ret = spi_bus_initialize(spi_cfg->host, &spi_cfg->buscfg, spi_cfg->dma_chan);
    PHY_CHECK(ret == ESP_OK, "init spi bus failed", err);

    /* Add device to spi bus */
    ret = spi_bus_add_device(spi_cfg->host, &spi_cfg->devcfg, &phy_spi->spi);
    PHY_CHECK(ret == ESP_OK, "add device to spi bus failed", err);
    phy->isInited = true;
    phy->dc = spi_cfg->dc;
    phy->rst = spi_cfg->rst;
    phy->blk = spi_cfg->blk;

    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t esp_lcd_phy_spi_deinit(esp_lcd_phy_t *phy)
{
    /* Get spi driver handle */
    lcd_phy_spi_t *phy_spi = __containerof(phy, lcd_phy_spi_t, parent);
    PHY_CHECK(phy_spi, "phy_spi is null", err);

    PHY_CHECK(phy->isInited, "spi hasn't been inited", err);

    /* Remove devices form spi bus and free bus*/
    PHY_CHECK(spi_bus_remove_device(phy_spi->spi) == ESP_OK, "remove device from spi bus failed", err);
    PHY_CHECK(spi_bus_free(phy_spi->host) == ESP_OK, "free bus failed", err);

    // TODO: deinit bus settings

    phy->isInited = false;

    return ESP_OK;

err:
    return ESP_FAIL;
}

static esp_err_t esp_lcd_phy_spi_write_cmd(esp_lcd_phy_t *phy, uint8_t cmd)
{
    /* Get spi driver handle */
    lcd_phy_spi_t *phy_spi = __containerof(phy, lcd_phy_spi_t, parent);
    PHY_CHECK(phy_spi, "phy_spi is null", err);

    /* Build sppi transmiton struct */
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.tx_buffer = &cmd;
    t.user = (void *)0;

    /* For faster speed, transalte by polling */
    PHY_CHECK(spi_device_polling_transmit(phy_spi->spi, &t) == ESP_OK, "Transmit data failed", err); /*transmited!*/
    return ESP_OK;

err:
    return ESP_FAIL;
}

static esp_err_t esp_lcd_phy_spi_write_data(esp_lcd_phy_t *phy, void *data, data_info_t info)
{
    /* Get spi driver handle */
    spi_transaction_t t;
    lcd_phy_spi_t *phy_spi = __containerof(phy, lcd_phy_spi_t, parent);

    /* Build sppi transmiton struct. dectect data length by configed in data */
    memset(&t, 0, sizeof(t));
    t.length = info.type * info.size;
    t.tx_buffer = data;
    t.user = (void *)1;

    /* For faster speed, transalte by polling */
    PHY_CHECK(spi_device_polling_transmit(phy_spi->spi, &t) == ESP_OK, "Transmit data failed", err); /*transmited!*/

    return ESP_OK;

err:
    return ESP_FAIL;
}

esp_lcd_phy_t *esp_lcd_phy_spi_factory()
{
    lcd_phy_spi_t *phy_spi = calloc(1, sizeof(lcd_phy_spi_t));
    PHY_CHECK(phy_spi, "calloc phy_spi failed", err);

    phy_spi->parent.isInited = false;
    phy_spi->parent.phy_init = esp_lcd_phy_spi_init;
    phy_spi->parent.phy_deinit = esp_lcd_phy_spi_deinit;
    phy_spi->parent.write_cmd = esp_lcd_phy_spi_write_cmd;
    phy_spi->parent.write_data = esp_lcd_phy_spi_write_data;

    return &(phy_spi->parent);

err:
    return NULL;
}
