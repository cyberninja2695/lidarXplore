#include <Arduino.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include <bsp_motor_i2c.h>
#include <i2c_module.h>
#include <ota_module.h>
#include <credentials.h>

#define FIRMWARE_VERSION 2

#define TEST

const char *ssid = SSID;
const char *password = PASSWORD;
const char *versionUrl = VERSIONURL;
const char *firmwareUrl = FIRMWAREURL;
const char *rootCA = ROOT_CA;

OTAUpdater ota(ssid, password, versionUrl, firmwareUrl, rootCA, FIRMWARE_VERSION);


#define delay_ms(ms) vTaskDelay(pdMS_TO_TICKS(ms)) // FreeRTOS millisecond delay macro

#define UPLOAD_DATA 2  //1: Receive total encoder data 2: Receive real-time encoder	   
#define MOTOR_TYPE 1   //1:520 motor 2:310 motor 3:speed code disc TT motor 4:TT DC reduction motor 5:L type 520 motor

void setup() {

	Serial.begin(115200);
	ota.begin();
    ota.updateIfAvailable();

	// Motor module iic communication initialization
	if (i2c_module_init() != ESP_OK) {
		ESP_LOGE("MAIN", "I2C initialization failed!");
		return;
	}
	
	printf("please wait...\r\n");
	
	init_motor_bsp(MOTOR_TYPE); //Initialize motor module, parameter: motor type
	delay_ms(100);

	pid_pos_control(90,1,1,1); //PID position control test, parameters: target position (degree), Kp, Ki, Kd

}

void loop() {

	#ifdef TEST
	//printf("Test mode ON ... \n");
	
	#else
	static int i;
	for(i=0;i<100;i++)
	{
		control_pwm(i*-20,i*-20,i*-20,-3500);// PWM control
		delay_ms(10);
		
		#if UPLOAD_DATA == 1
		read_all_encoder();
		//printf("M1:%d\t M2:%d\t M3:%d\t M4:%d\t \r\n",Encoder_Now[0],Encoder_Now[1],Encoder_Now[2],Encoder_Now[3]);
		printf("M4_now:%d,\t M4_corrected:%d\t \r\n",Encoder_Now[3], Encoder_Now[3]-encoder_start);
		#elif UPLOAD_DATA == 2
		read_10_encoder();
		//printf("M1:%d\t M2:%d\t M3:%d\t M4:%d\t \r\n",Encoder_Offset[0],Encoder_Offset[1],Encoder_Offset[2],Encoder_Offset[3]);
		printf("M4_offset:%d\t \r\n",Encoder_Offset[3]);
		#endif
	}
	#endif
  

}