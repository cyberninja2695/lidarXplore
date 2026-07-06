#include "i2c_module.h"
#include "esp_log.h"  //logging library for debugging

//Define log tag for debugging output
static const char *TAG = "i2c_module";

//Initialize I2C interface
esp_err_t i2c_module_init(void)
{
    //Define I2C configuration structure
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,         //Set as master mode
        .sda_io_num = I2C_MASTER_SDA_IO, //SDA pin GPIO21
        .scl_io_num = I2C_MASTER_SCL_IO, //SCL pin GPIO22
        .sda_pullup_en = GPIO_PULLUP_ENABLE, //Enable SDA pull-up resistor
        .scl_pullup_en = GPIO_PULLUP_ENABLE, //SCL Enable SCL pull-up resistor
        .master {.clk_speed = I2C_MASTER_FREQ_HZ}, //Set I2C clock frequency
    };

    //Apply I2C parameter configuration
    esp_err_t err = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C param config failed: %s", esp_err_to_name(err));
        return err;
    }

    //Install I2C driver
    err = i2c_driver_install(I2C_MASTER_NUM, I2C_MODE_MASTER, 0, 0, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C driver install failed: %s", esp_err_to_name(err));
    }

    return err;
}

//Write data to I2C device
esp_err_t i2cWrite(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *data)
{
    //Create I2C command queue
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    //Start signal
    i2c_master_start(cmd);

    //Send device address + write bit
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);

    //Send register address
    i2c_master_write_byte(cmd, reg, true);

    //Send data
    i2c_master_write(cmd, data, len, true);

    // ֹͣStop signal
    i2c_master_stop(cmd);

    //Execute I2C command
    esp_err_t err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);

    //Delete I2C command queue
    i2c_cmd_link_delete(cmd);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C write failed: %s", esp_err_to_name(err));
    }

    return err;
}

//Read data from I2C device
esp_err_t i2cRead(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    // Create I2C command queue
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    // Start signal
    i2c_master_start(cmd);

    //Send device address + write bit
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);

    //Send register address
    i2c_master_write_byte(cmd, reg, true);

    //Repeated start signal
    i2c_master_start(cmd);

    //Send device address + read bit
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_READ, true);

    //Read data
    if (len > 1) {
        i2c_master_read(cmd, buf, len - 1, I2C_MASTER_ACK); //Read n-1 bytes with ACK
    }
    i2c_master_read_byte(cmd, buf + len - 1, I2C_MASTER_NACK); //Send NACK after the last byte

    //Stop signal
    i2c_master_stop(cmd);

    //Execute I2C command
    esp_err_t err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);

    //Delete I2C command queue
    i2c_cmd_link_delete(cmd);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C read failed: %s", esp_err_to_name(err));
    }

    return err;
}

