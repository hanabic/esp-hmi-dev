#include "string.h"
#include "esp_lcd_phy.h"
#include "driver/gpio.h"

esp_err_t esp_lcd_phy_spi_write_cmd(esp_lcd_phy_t *phy, uint8_t cmd)
{
    esp_err_t ret;
    spi_transaction_t t;
    lcd_phy_spi_t *phy_spi = __containerof(phy, lcd_phy_spi_t, parent);

    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.tx_buffer = &cmd;
    t.user = (void *)0;
    ret = spi_device_polling_transmit(phy_spi->spi, &t);
    assert(ret == ESP_OK);
    return ret;
}

esp_err_t esp_lcd_phy_spi_init(esp_lcd_phy_t *phy, void *cfg)
{
    esp_err_t ret;
    lcd_phy_spi_t *phy_spi = __containerof(phy, lcd_phy_spi_t, parent);
    lcd_phy_spi_config_t *spi_cfg = (lcd_phy_spi_config_t *)cfg;

    ret = spi_bus_initialize(spi_cfg->host, &spi_cfg->buscfg, spi_cfg->dma_chan);
    ESP_ERROR_CHECK(ret);
    ret = spi_bus_add_device(spi_cfg->host, &spi_cfg->devcfg, &phy_spi->spi);
    ESP_ERROR_CHECK(ret);
    assert(ret == ESP_OK);

    return ret;
}

esp_err_t esp_lcd_phy_spi_deinit(esp_lcd_phy_t *phy)
{
    esp_err_t ret;
    lcd_phy_spi_t *phy_spi = __containerof(phy, lcd_phy_spi_t, parent);
    ret = spi_bus_remove_device(phy_spi->spi);

    assert(ret == ESP_OK);
    return ret;
}

esp_err_t esp_lcd_phy_spi_write_data(esp_lcd_phy_t *phy, esp_lcd_data_t data)
{
    esp_err_t ret;
    spi_transaction_t t;
    lcd_phy_spi_t *phy_spi = __containerof(phy, lcd_phy_spi_t, parent);

    memset(&t, 0, sizeof(t));
    if (data.type == DATA_TYPE_UINT8) {
        t.length = data.size * 8;
        t.tx_buffer = (void *)data.buf_ptr.buf_uint8;
    } else if (data.type == DATA_TYPE_UINT8) {
        t.length = data.size * 16;
        t.tx_buffer = (void *)data.buf_ptr.buf_uint16;
    }

    t.user = (void *)1;
    ret = spi_device_polling_transmit(phy_spi->spi, &t);
    assert(ret == ESP_OK);
    return ret;
}

esp_lcd_phy_t *esp_lcd_phy_spi_factory(const lcd_phy_spi_config_t *cfg)
{
    lcd_phy_spi_t *phy_spi = calloc(1, sizeof(lcd_phy_spi_t));

    phy_spi->parent.type == LCD_PHY_TYPE_SPI;
    phy_spi->parent.base_data_type == DATA_TYPE_UINT8;
    phy_spi->parent.phy_init = esp_lcd_phy_spi_init;
    phy_spi->parent.phy_deinit = esp_lcd_phy_spi_deinit;
    phy_spi->parent.write_cmd = esp_lcd_phy_spi_write_cmd;
    phy_spi->parent.write_data = esp_lcd_phy_spi_write_data;

    return &(phy_spi->parent);
}
