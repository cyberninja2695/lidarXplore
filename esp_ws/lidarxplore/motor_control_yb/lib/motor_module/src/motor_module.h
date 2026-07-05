#ifndef __bsp_motor_i2c_H_
#define __bsp_motor_i2c_H_

#include <Arduino.h>
#include <string.h>
#include "i2c_module.h"

#define Motor_model_ADDR    0x26


class Motor {
    private:
        void set_motor_num(uint8_t data);
        void set_motor_type(uint8_t data);
        void set_motor_deadzone(uint16_t data);
        void set_pulse_line(uint16_t data);
        void set_pulse_phase(uint16_t data);
        void set_wheel_dia(float data);

    public:
        Motor(uint8_t motor_num, uint8_t motor_type, uint16_t pulse_phase, uint16_t pulse_line, float wheel_dia, uint16_t motor_deadzone);
        ~Motor();
        
        void control_speed(int16_t m1,int16_t m2 ,int16_t m3,int16_t m4);
        void control_pwm(int16_t m1,int16_t m2 ,int16_t m3,int16_t m4);
        void read_10_encoder(void);
        void read_all_encoder(void);

};


typedef enum __Motor_IIC_ADDR_
{
	//Wirte Reg
	MOTOR_TYPE_REG = 0x01, //�������	Motor Type
	MOTOR_DeadZONE_REG = 0x02, //��������	Dead zone configuration
	MOTOR_PulseLine_REG = 0x03, //�Ż�����	Magnetic loop number
	MOTOR_PulsePhase_REG = 0x04, //���ٱ�	Reduction ratio
	WHEEL_DIA_REG = 0x05, //����ֱ��	Wheel diameter
	SPEED_Control_REG = 0x06, //�ٶȿ���	Speed control
	PWM_Control_REG = 0x07, //pwm����	PWM control
	
	
	//Read Reg
	READ_TEN_M1Enconer_REG = 0x10, //10msʵʱ��������ȡM1	10ms real-time encoder reading M1
	READ_TEN_M2Enconer_REG = 0x11, //10msʵʱ��������ȡM2	10ms real-time encoder reading M2
	READ_TEN_M3Enconer_REG = 0x12, //10msʵʱ��������ȡM3	10ms real-time encoder reading M3
	READ_TEN_M4Enconer_REG = 0x13, //10msʵʱ��������ȡM4	10ms real-time encoder reading M4
	
	READ_ALLHigh_M1_REG = 0x20, //��ȡM1�ܵ������� ��λ	Read the total number of pulses of M1 High
	READ_ALLLOW_M1_REG = 0x21, //��ȡM1�ܵ������� ��λ	Read the total number of pulses of M1 low
	
	READ_ALLHigh_M2_REG = 0x22, //��ȡM2�ܵ������� ��λ	Read the total number of pulses of M2 High
	READ_ALLLOW_M2_REG = 0x23,  //��ȡM2�ܵ������� ��λ	Read the total number of pulses of M2 low
	
	READ_ALLHigh_M3_REG = 0x24, //��ȡM3�ܵ������� ��λ	Read the total number of pulses of M3 High
	READ_ALLLOW_M3_REG = 0x25, //��ȡM3�ܵ������� ��λ	Read the total number of pulses of M3 low
	
	READ_ALLHigh_M4_REG = 0x26, //��ȡM4�ܵ������� ��λ	Read the total number of pulses of M4 High
	READ_ALLLOW_M4_REG = 0x27, //��ȡM4�ܵ������� ��λ	Read the total number of pulses of M4 low
	
	IIC_REG_MAX //����	The largest
	

}Motor_IIC_ADDR_t;


//�������������������ⲿʹ��	Lead out encoder variables for external use
extern int16_t  Encoder_Offset[4];
extern int Encoder_Now[4];






#endif
