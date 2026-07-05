#include "motor_module.h"
#include "motor_constants.h"

Motor::Motor(uint8_t motor_num, uint8_t motor_type, uint16_t pulse_phase, uint16_t pulse_line, float wheel_dia, uint16_t motor_deadzone) {
    set_motor_num(motor_num);
    set_motor_type(motor_type);
    set_pulse_phase(pulse_phase);
    set_pulse_line(pulse_line);
    set_wheel_dia(wheel_dia);
    set_motor_deadzone(motor_deadzone);
}

Motor::~Motor() {
    // Destructor logic if needed
}

void Motor::set_motor_num(uint8_t data)
{	
    READ_TEN_M1Enconer_REG = 0x10, //10msʵʱ��������ȡM1	10ms real-time encoder reading M1

}

int16_t  Encoder_Offset[4];
int Encoder_Now[4];

  
//������תbytesλ //bytes 4������ ��Ϊfloat��4�ֽ�	Convert float to bytes //bytes 4 in length because float is 4 bytes
void float_to_bytes(float f, uint8_t *bytes) 
{
    memcpy(bytes, &f, sizeof(float));
}

//bytesλת�ɸ�����	Convert bytes to floating point
float char2float(char *p)
{
  float *p_Int;
  p_Int = (float *)malloc(sizeof(float));
  memcpy(p_Int, p, sizeof(float));
  float x = *p_Int;
  free(p_Int);
  return x;
}

//���õ��	Configure the motor
void set_motor_type(uint8_t data)
{	
	i2cWrite(Motor_model_ADDR,MOTOR_TYPE_REG,2,&data);
}

//��������	Configuring Dead Zone
void set_motor_deadzone(uint16_t data)
{
	static uint8_t buf_tempzone[2];
	
	buf_tempzone[0] = (data>>8)&0xff;
	buf_tempzone[1] = data;
	
	i2cWrite(Motor_model_ADDR,MOTOR_DeadZONE_REG,2,buf_tempzone);
}

//���ôŻ���	Configuring magnetic loop
void set_pulse_line(uint16_t data)
{
	static uint8_t buf_templine[2];
	
	buf_templine[0] = (data>>8)&0xff;
	buf_templine[1] = data;
	
	i2cWrite(Motor_model_ADDR,MOTOR_PulseLine_REG,2,buf_templine);
}

//���ü��ٱ�	Configure the reduction ratio
void set_pulse_phase(uint16_t data)
{
	static uint8_t buf_tempPhase[2];
	
	buf_tempPhase[0] = (data>>8)&0xff;
	buf_tempPhase[1] = data;
	
	i2cWrite(Motor_model_ADDR,MOTOR_PulsePhase_REG,2,buf_tempPhase);
}


//����ֱ��	Configuration Diameter
void set_wheel_dia(float data)
{
	static uint8_t bytes[4];
	
	float_to_bytes(data,bytes);
	
	i2cWrite(Motor_model_ADDR,WHEEL_DIA_REG,4,bytes);
}

//ֻ�ܿ��ƴ����������͵ĵ��	Can only control motors with encoders
//�������:4��������ٶ�		Input parameters: speed of 4 motors
void control_speed(int16_t m1,int16_t m2 ,int16_t m3,int16_t m4)
{
	static uint8_t speed[8];
	
	speed[0] = (m1>>8)&0xff;
	speed[1] = (m1)&0xff;
	
	speed[2] = (m2>>8)&0xff;
	speed[3] = (m2)&0xff;
	
	speed[4] = (m3>>8)&0xff;
	speed[5] = (m3)&0xff;
	
	speed[6] = (m4>>8)&0xff;
	speed[7] = (m4)&0xff;
	
	i2cWrite(Motor_model_ADDR,SPEED_Control_REG,8,speed);

}


//���ƴ����������͵ĵ��	Control the motor with encoder type
//�������:4�������pwm	PWM of 4 motors
//�˺������Խ��ʵʱ�����������ݣ���ʵ��control_speed�Ĺ���	This function can combine the data of real-time encoder to realize the function of control_speed
void control_pwm(int16_t m1,int16_t m2 ,int16_t m3,int16_t m4)
{
	static uint8_t pwm[8];
	
	pwm[0] = (m1>>8)&0xff;
	pwm[1] = (m1)&0xff;
	
	pwm[2] = (m2>>8)&0xff;
	pwm[3] = (m2)&0xff;
	
	pwm[4] = (m3>>8)&0xff;
	pwm[5] = (m3)&0xff;
	
	pwm[6] = (m4>>8)&0xff;
	pwm[7] = (m4)&0xff;
	
	i2cWrite(Motor_model_ADDR,PWM_Control_REG,8,pwm);

}


//��ȡ���ʱ��ı����������� 10ms��	Read the data of the encoder of relative time 10ms
void read_10_encoder(void)
{
	static uint8_t buf[2];
		
	//M1���������������	M1 motor encoder data
	i2cRead(Motor_model_ADDR, READ_TEN_M1Enconer_REG, 2, buf);
	Encoder_Offset[0] = buf[0]<<8|buf[1]; 
	
	//M2���������������	M2 motor encoder data
	i2cRead(Motor_model_ADDR, READ_TEN_M2Enconer_REG, 2, buf);
	Encoder_Offset[1] = buf[0]<<8|buf[1];
	
	//M3���������������	M3 motor encoder data
	i2cRead(Motor_model_ADDR, READ_TEN_M3Enconer_REG, 2, buf);
	Encoder_Offset[2] = buf[0]<<8|buf[1];
	
	//M4���������������	M4 motor encoder data
	i2cRead(Motor_model_ADDR, READ_TEN_M4Enconer_REG, 2, buf);
	Encoder_Offset[3] = buf[0]<<8|buf[1];
	
}

//��ȡ���ת���ı���������	Read the encoder data of the motor rotation
void read_all_encoder(void)
{
	static uint8_t buf[2];
	static uint8_t buf2[2];
	
	//M1���������������	M1 motor encoder data
	i2cRead(Motor_model_ADDR, READ_ALLHigh_M1_REG, 2, buf);
	i2cRead(Motor_model_ADDR, READ_ALLLOW_M1_REG, 2, buf2);
	
	Encoder_Now[0] = buf[0]<<24|buf[1]<<16|buf2[0]<<8|buf2[1]; 
	
	//M2���������������	M2 motor encoder data
	i2cRead(Motor_model_ADDR, READ_ALLHigh_M2_REG, 2, buf);
	i2cRead(Motor_model_ADDR, READ_ALLLOW_M2_REG, 2, buf2);
	Encoder_Now[1] = buf[0]<<24|buf[1]<<16|buf2[0]<<8|buf2[1];
	
	//M3���������������	M3 motor encoder data
	i2cRead(Motor_model_ADDR, READ_ALLHigh_M3_REG, 2, buf);
	i2cRead(Motor_model_ADDR, READ_ALLLOW_M3_REG, 2, buf2);
	Encoder_Now[2] = buf[0]<<24|buf[1]<<16|buf2[0]<<8|buf2[1];
	
	
	//M4���������������	M4 motor encoder data
	i2cRead(Motor_model_ADDR, READ_ALLHigh_M4_REG, 2, buf);
	i2cRead(Motor_model_ADDR, READ_ALLLOW_M4_REG, 2, buf2);
	Encoder_Now[3] = buf[0]<<24|buf[1]<<16|buf2[0]<<8|buf2[1];
	
}
