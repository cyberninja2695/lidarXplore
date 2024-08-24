#include <Arduino.h>
#include <Wire.h>

const int ESP_SDA = 10;
const int ESP_SCL = 8;


/**
 * @file DriverSample.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief Module 4EncoderMotor Test Demo.
 * @version 0.1
 * @date 2024-01-19
 *
 *
 * @Hardwares: M5Core/Core2/CoreS3 + Module 4EncoderMotor
 * @Platform Version: Arduino M5Stack Board Manager v2.1.0
 * @Dependent Library:
 * M5Unified: https://github.com/m5stack/M5Unified
 * M5GFX: https://github.com/m5stack/M5GFX
 * M5Module4EncoderMotor: https://github.com/m5stack/M5Module-4EncoderMotor
 */

#include "M5Unified.h"
#include "M5GFX.h"
#include "M5Module4EncoderMotor.h"

M5Module4EncoderMotor driver;

#define MAX_RECORD_SIZE 256

float amp_record[MAX_RECORD_SIZE] = {0};
uint8_t record_index              = 0;
float amp_value                   = 0.0f;

uint8_t avg_filter_level = 20;

float avg_filter(float *data, int len) {
    float sum = 0;
    float min = data[0];
    float max = data[0];
    for (int i = 0; i < len; i++) {
        if (data[i] < min) {
            min = data[i];
        }
        if (data[i] > max) {
            max = data[i];
        }
        sum += data[i];
    }
    sum -= min;
    sum -= max;
    return sum / (len - 2);
}

void setup() {

    //Wire.setPins(ESP_SDA, ESP_SCL);
    //Wire.begin();

    Serial.begin(115200);
    while(!Serial){}

    M5.begin();

    /*
    m5::board_t board = M5.getBoard();
    switch (board) {
        case m5::board_t::board_M5Stack: {
            //while (!driver.begin(&Wire, MODULE_4ENCODERMOTOR_ADDR, 21, 22)) {
            while (!driver.begin(&Wire, MODULE_4ENCODERMOTOR_ADDR, ESP_SDA, ESP_SCL)) {  
                Serial.println("Driver Init faild!");
                delay(1000);
            }
            break;
        }
        case m5::board_t::board_M5StackCore2: {
            //while (!driver.begin(&Wire1, MODULE_4ENCODERMOTOR_ADDR, 21, 22)) {
            while (!driver.begin(&Wire1, MODULE_4ENCODERMOTOR_ADDR, ESP_SDA, ESP_SCL)) {      
                Serial.println("Driver Init failed!");
                M5.Display.drawString("Driver Init faild!", 160, 7);
                delay(1000);
            }
            break;
        }
        case m5::board_t::board_M5StackCoreS3: {
            //while (!driver.begin(&Wire1, MODULE_4ENCODERMOTOR_ADDR, 11, 12)) {
            while (!driver.begin(&Wire1, MODULE_4ENCODERMOTOR_ADDR, ESP_SDA, ESP_SCL)) {
                Serial.println("Driver Init faild!");
                M5.Display.drawString("Driver Init failed!", 160, 7);
                delay(1000);
            }
            break;
        }
        default:
            Serial.println("Driver Init failed!");
            return;
            break;
    }
    */
    
    while(!driver.begin(&Wire, MODULE_4ENCODERMOTOR_ADDR, ESP_SDA, ESP_SCL)) {
        Serial.println("Driver Init failed!");
    };
    Serial.println("Driver Init success!");

    // motor channel 0 -3
    //for (uint8_t i = 0; i < 4; i++) {
        ///*
        driver.setMode(3, NORMAL_MODE);
        driver.setMotorSpeed(3, 127);
        //*/
        
        ///*
        driver.setMode(3, SPEED_MODE);
        driver.setSpeedPoint(3, 127);
        //*/

        M5.update();
        
    //}
}

bool direction = true;
//int mode       = NORMAL_MODE;

void loop() {
    //M5.update();
    if (avg_filter_level != 0) {
        amp_record[record_index] = driver.getMotorCurrent();
        record_index++;
        if (record_index >= avg_filter_level) {
            record_index = 0;
        }
        amp_value = avg_filter(amp_record, avg_filter_level);
    }

    //for (uint8_t i = 0; i < 4; i++) {
        int32_t encoder_value = driver.getEncoderValue(3);
        Serial.print("encoder3_value = "); Serial.println(encoder_value);
    //}

    //driver.setSpeedPoint(3, 255);

}
