#ifndef __bsp_motor_i2c_H_
#define __bsp_motor_i2c_H_

#include <Arduino.h>
#include <string.h>
#include "i2c_module.h"

#define Motor_model_ADDR    0x26


typedef enum __Motor_IIC_ADDR_
{
	//Wirte Reg
	MOTOR_TYPE_REG = 0x01, //Motor Type
	MOTOR_DeadZONE_REG = 0x02, //Dead zone configuration
	MOTOR_PulseLine_REG = 0x03, //Magnetic loop number
	MOTOR_PulsePhase_REG = 0x04, //Reduction ratio
	WHEEL_DIA_REG = 0x05, //Wheel diameter
	SPEED_Control_REG = 0x06, //Speed control
	PWM_Control_REG = 0x07, //PWM control
	
	
	//Read Reg
	READ_TEN_M1Encoder_REG = 0x10, //10ms real-time encoder reading M1
	READ_TEN_M2Encoder_REG = 0x11, //10ms real-time encoder reading M2
	READ_TEN_M3Encoder_REG = 0x12, //10ms real-time encoder reading M3
	READ_TEN_M4Encoder_REG = 0x13, //10ms real-time encoder reading M4
	
	READ_ALLHigh_M1_REG = 0x20, //Read the total number of pulses of M1 High
	READ_ALLLOW_M1_REG = 0x21, //Read the total number of pulses of M1 low
	
	READ_ALLHigh_M2_REG = 0x22, //Read the total number of pulses of M2 High
	READ_ALLLOW_M2_REG = 0x23,  //Read the total number of pulses of M2 low
	
	READ_ALLHigh_M3_REG = 0x24, //Read the total number of pulses of M3 High
	READ_ALLLOW_M3_REG = 0x25, //Read the total number of pulses of M3 low
	
	READ_ALLHigh_M4_REG = 0x26, //Read the total number of pulses of M4 High
	READ_ALLLOW_M4_REG = 0x27, //Read the total number of pulses of M4 low
	
	IIC_REG_MAX //The largest
	

}Motor_IIC_ADDR_t;


//Lead out encoder variables for external use
extern int16_t  Encoder_Offset[4];
extern int Encoder_Now[4];


void control_speed(int16_t m1,int16_t m2 ,int16_t m3,int16_t m4);
void control_pwm(int16_t m1,int16_t m2 ,int16_t m3,int16_t m4);
void set_motor_type(uint8_t data);
void read_10_encoder(void);
void read_all_encoder(void);
void set_motor_deadzone(uint16_t data);
void set_pulse_line(uint16_t data);
void set_pulse_phase(uint16_t data);
void set_wheel_dia(float data);

void init_motor_bsp(uint8_t motor_type);
int read_encoder_start();
int32_t count_to_degree(int32_t count, uint16_t pulse_phase);
int32_t degree_to_count(int32_t degree, uint16_t pulse_sphase);
int calc_pwm_input(int pid_error);
void pid_pos_control(int target_deg, int Kp, int Ki, int Kd);


#endif
