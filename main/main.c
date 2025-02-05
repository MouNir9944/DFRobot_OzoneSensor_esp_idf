#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c_master.h"

#define COLLECT_NUMBER  20 

// I2C Address for Ozone Sensor
#define Ozone_IICAddress 0x73 

#define           OZONE_ADDRESS_0           0x70
#define           OZONE_ADDRESS_1           0x71
#define           OZONE_ADDRESS_2           0x72
#define           OZONE_ADDRESS_3           0x73
#define           MEASURE_MODE_AUTOMATIC    0x00           ///< active  mode
#define           MEASURE_MODE_PASSIVE      0x01           ///< passive mode
#define           AUTO_READ_DATA            0x00           ///< auto read ozone data
#define           PASSIVE_READ_DATA         0x01           ///< passive read ozone data
#define           MODE_REGISTER             0x03           ///< mode register
#define           SET_PASSIVE_REGISTER      0x04           ///< read ozone data register
#define           AUTO_DATA_HIGE_REGISTER   0x09           ///< AUTO data high eight bits
#define           AUTO_DATA_LOW_REGISTER    0x0A           ///< AUTO data Low  eight bits
#define           PASS_DATA_HIGE_REGISTER   0x07           ///< AUTO data high eight bits
#define           PASS_DATA_LOW_REGISTER    0x08           ///< AUTO data Low  eight bits
#define           OCOUNT                    100            ///< Ozone Count Value
// I2C Address for Ozone Sensor
#define Ozone_IICAddress OZONE_ADDRESS_3
uint8_t _M_Flag = 0;
  int ozoneData[OCOUNT] = {0x00};

// I2C Configurations
#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000 

i2c_master_dev_handle_t SEN0321_handle;

// Function to initialize I2C
void init_i2c()
{
    i2c_master_bus_config_t bus_config = {
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .i2c_port = I2C_MASTER_NUM,
        .flags.enable_internal_pullup = true,
        .glitch_ignore_cnt = 7,
        .clk_source = I2C_CLK_SRC_APB,
    };

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = Ozone_IICAddress,
        .scl_speed_hz = I2C_MASTER_FREQ_HZ,
        .scl_wait_us = 1000,
        .flags.disable_ack_check = false,
    };

    i2c_master_bus_handle_t i2c_master_bus;
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &i2c_master_bus));
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_master_bus, &dev_config, &SEN0321_handle));
}

// Function to set Ozone sensor mode
void setModes(i2c_master_dev_handle_t SEN0321_handle, uint8_t mode)
{
    uint8_t buff[2] = {MODE_REGISTER, mode};  // Corrected buffer initialization

    esp_err_t ret = i2c_master_transmit(SEN0321_handle, buff, sizeof(buff), -1);
    
    if (ret == ESP_OK) {
        ESP_LOGI("I2C", "Mode set successfully: %s", mode == MEASURE_MODE_AUTOMATIC ? "Automatic" : "Passive");
        _M_Flag = mode;
    } else {
        ESP_LOGE("I2C", "Failed to set mode: %s", esp_err_to_name(ret));
    }

    vTaskDelay(pdMS_TO_TICKS(100));  // Corrected delay function
}

int getAverageNum(int bArray[], int iFilterLen)
{
  unsigned long bTemp = 0;
  for(uint16_t i = 0; i < iFilterLen; i++) {
    bTemp += bArray[i];
  }
  return bTemp / iFilterLen;
}

int16_t readOzoneData(uint8_t collectNum)
{
    static uint8_t i = 0, j = 0;
    if (collectNum > 0) {
        for (j = collectNum - 1; j > 0; j--) {
            ozoneData[j] = ozoneData[j - 1];
        }

        uint8_t high_byte, low_byte;
        esp_err_t ret;

        if (_M_Flag == 0) {  // Automatic Mode
            // Send command to set auto-read mode
            uint8_t buff[2] = {SET_PASSIVE_REGISTER, AUTO_READ_DATA};
            ret = i2c_master_transmit(SEN0321_handle, buff, sizeof(buff), portMAX_DELAY);
            if (ret != ESP_OK) return -1;

            vTaskDelay(pdMS_TO_TICKS(100));  // 100ms delay

            // Read high byte
            uint8_t cmd = AUTO_DATA_HIGE_REGISTER;
            ret = i2c_master_transmit(SEN0321_handle, &cmd, 1, portMAX_DELAY);
            if (ret != ESP_OK) return -1;
            ret = i2c_master_receive(SEN0321_handle, &high_byte, 1, portMAX_DELAY);
            if (ret != ESP_OK) return -1;

            // Read low byte
            cmd = AUTO_DATA_LOW_REGISTER;
            ret = i2c_master_transmit(SEN0321_handle, &cmd, 1, portMAX_DELAY);
            if (ret != ESP_OK) return -1;
            ret = i2c_master_receive(SEN0321_handle, &low_byte, 1, portMAX_DELAY);
            if (ret != ESP_OK) return -1;

        } else {  // Passive Mode
            // Send request to read passive data
            uint8_t buff[2] = {SET_PASSIVE_REGISTER, PASSIVE_READ_DATA};
            ret = i2c_master_transmit(SEN0321_handle, buff, sizeof(buff), portMAX_DELAY);
            if (ret != ESP_OK) return -1;

            vTaskDelay(pdMS_TO_TICKS(100));  // 100ms delay

            // Read high byte
            uint8_t cmd = PASS_DATA_HIGE_REGISTER;
            ret = i2c_master_transmit(SEN0321_handle, &cmd, 1, portMAX_DELAY);
            if (ret != ESP_OK) return -1;
            ret = i2c_master_receive(SEN0321_handle, &high_byte, 1, portMAX_DELAY);
            if (ret != ESP_OK) return -1;

            // Read low byte
            cmd = PASS_DATA_LOW_REGISTER;
            ret = i2c_master_transmit(SEN0321_handle, &cmd, 1, portMAX_DELAY);
            if (ret != ESP_OK) return -1;
            ret = i2c_master_receive(SEN0321_handle, &low_byte, 1, portMAX_DELAY);
            if (ret != ESP_OK) return -1;
        }

        ozoneData[0] = (high_byte << 8) | low_byte;  // Combine high and low bytes

        if (i < collectNum) i++;
        return getAverageNum(ozoneData, i);
    } else if (collectNum <= 0 || collectNum > 100) {
        return -1;
    }

    return 0;
}

void app_main(void)
{
    init_i2c();
    setModes(SEN0321_handle, MEASURE_MODE_PASSIVE);//MEASURE_MODE_AUTOMATIC
    while(1){

        int16_t ozoneConcentration = readOzoneData(COLLECT_NUMBER);
        ESP_LOGI("TAG","Ozone concentration is %d PPB.",ozoneConcentration);
        vTaskDelay(pdMS_TO_TICKS(1000));  // Corrected delay function
    }
}
