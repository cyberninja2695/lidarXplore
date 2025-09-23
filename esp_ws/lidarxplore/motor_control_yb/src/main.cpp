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

OTAUpdater ota(ssid, password, versionUrl, firmwareUrl, FIRMWARE_VERSION);


#define delay_ms(ms) vTaskDelay(pdMS_TO_TICKS(ms)) // FreeRTOS millisecond delay macro

#define UPLOAD_DATA 2  //1: Receive total encoder data 2: Receive real-time encoder	   
#define MOTOR_TYPE 2   //1:520 motor 2:310 motor 3:speed code disc TT motor 4:TT DC reduction motor 5:L type 520 motor
                       

void setup() {

	Serial.begin(115200);
	ota.begin();
    ota.updateIfAvailable();

	#ifdef TEST
	printf("Test mode ON ... \n");

	#else
	// Motor module iic communication initialization
	if (i2c_module_init() != ESP_OK) {
		ESP_LOGE("MAIN", "I2C initialization failed!");
		return;
	}
	
	printf("please wait...\r\n");
	
	#if MOTOR_TYPE == 1
	Set_motor_type(1);//配置电机类型	Configure motor type
	delay_ms(100);
	Set_Pulse_Phase(30);//配置减速比 查电机手册得出	Configure the reduction ratio. Check the motor manual to find out
	delay_ms(100);
	Set_Pulse_line(11);//配置磁环线 查电机手册得出	Configure the magnetic ring wire. Check the motor manual to get the result.
	delay_ms(100);
	Set_Wheel_dis(67.00);//配置轮子直径,测量得出		Configure the wheel diameter and measure it
	delay_ms(100);
	Set_motor_deadzone(1600);//配置电机死区,实验得出	Configure the motor dead zone, and the experiment shows
	delay_ms(100);
	
	#elif MOTOR_TYPE == 2
	Set_motor_type(2);
	delay_ms(100);
	Set_Pulse_Phase(20);
	delay_ms(100);
	Set_Pulse_line(13);
	delay_ms(100);
	Set_Wheel_dis(48.00);
	delay_ms(100);
	Set_motor_deadzone(1300);
	delay_ms(100);
	
	#elif MOTOR_TYPE == 3
	Set_motor_type(3);
	delay_ms(100);
	Set_Pulse_Phase(45);
	delay_ms(100);
	Set_Pulse_line(13);
	delay_ms(100);
	Set_Wheel_dis(68.00);
	delay_ms(100);
	Set_motor_deadzone(1250);
	delay_ms(100);
	
	#elif MOTOR_TYPE == 4
	Set_motor_type(4);
	delay_ms(100);
	Set_Pulse_Phase(48);
	delay_ms(100);
	Set_motor_deadzone(1000);
	delay_ms(100);
	
	#elif MOTOR_TYPE == 5
	Set_motor_type(1);
	delay_ms(100);
	Set_Pulse_Phase(40);
	delay_ms(100);
	Set_Pulse_line(11);
	delay_ms(100);
	Set_Wheel_dis(67.00);
	delay_ms(100);
	Set_motor_deadzone(1600);
	delay_ms(100);
	#endif

	#endif

	

}

void loop() {

	#ifdef TEST
	//Test
	//printf("Test mode ON ... \n");
	#else
	static int i;
	for(i=0;i<100;i++)
	{
		// Select control mode by motor type
		#if MOTOR_TYPE == 4
		control_pwm(i*20,i*20,i*20,i*20);// PWM control
		#else
		control_speed(i*10,i*10,i*10,i*10);// Speed control
		#endif

		delay_ms(100);
		
		#if UPLOAD_DATA == 1
		Read_ALL_Enconder();
		printf("M1:%d\t M2:%d\t M3:%d\t M4:%d\t \r\n",Encoder_Now[0],Encoder_Now[1],Encoder_Now[2],Encoder_Now[3]);
		#elif UPLOAD_DATA == 2
		Read_10_Enconder();
		printf("M1:%d\t M2:%d\t M3:%d\t M4:%d\t \r\n",Encoder_Offset[0],Encoder_Offset[1],Encoder_Offset[2],Encoder_Offset[3]);
		#endif
	}
	#endif
  

}