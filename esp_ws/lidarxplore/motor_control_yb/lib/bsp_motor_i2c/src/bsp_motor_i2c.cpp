#include "bsp_motor_i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#define PWM_MIN  -5000
#define PWM_MAX  5000
#define delay_ms(ms) vTaskDelay(pdMS_TO_TICKS(ms)) // FreeRTOS millisecond delay macro

int16_t  Encoder_Offset[4];
int Encoder_Now[4];
static uint16_t pulse_phase; //Default reduction ratio
static int encoder_start; //Encoder initial value
  
//Convert float to bytes //bytes 4 in length because float is 4 bytes
void float_to_bytes(float f, uint8_t *bytes) 
{
    memcpy(bytes, &f, sizeof(float));
}

//Convert bytes to floating point
float char2float(char *p)
{
  float *p_Int;
  p_Int = (float *)malloc(sizeof(float));
  memcpy(p_Int, p, sizeof(float));
  float x = *p_Int;
  free(p_Int);
  return x;
}

//Configure the motor
void set_motor_type(uint8_t data)
{	
	i2cWrite(Motor_model_ADDR,MOTOR_TYPE_REG,2,&data);
}

//Configuring Dead Zone
void set_motor_deadzone(uint16_t data)
{
	static uint8_t buf_tempzone[2];
	
	buf_tempzone[0] = (data>>8)&0xff;
	buf_tempzone[1] = data;
	
	i2cWrite(Motor_model_ADDR,MOTOR_DeadZONE_REG,2,buf_tempzone);
}

//Configuring magnetic loop
void set_pulse_line(uint16_t data)
{
	static uint8_t buf_templine[2];
	
	buf_templine[0] = (data>>8)&0xff;
	buf_templine[1] = data;
	
	i2cWrite(Motor_model_ADDR,MOTOR_PulseLine_REG,2,buf_templine);
}

//Configure the reduction ratio
void set_pulse_phase(uint16_t data)
{
	static uint8_t buf_tempPhase[2];
	
	buf_tempPhase[0] = (data>>8)&0xff;
	buf_tempPhase[1] = data;
	
	i2cWrite(Motor_model_ADDR,MOTOR_PulsePhase_REG,2,buf_tempPhase);

	pulse_phase = data;
}


//Configuration Diameter
void set_wheel_dia(float data)
{
	static uint8_t bytes[4];
	
	float_to_bytes(data,bytes);
	
	i2cWrite(Motor_model_ADDR,WHEEL_DIA_REG,4,bytes);
}

//Can only control motors with encoders
//Input parameters: speed of 4 motors
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


//Control the motor with encoder type
//Input parameters:4 motors PWM
//This function can combine the data of real-time encoder to realize the function of control_speed
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


//Read the data of the encoder of relative time 10ms
void read_10_encoder(void)
{
	static uint8_t buf[2];
		
	//M1 motor encoder data
	i2cRead(Motor_model_ADDR, READ_TEN_M1Enconer_REG, 2, buf);
	Encoder_Offset[0] = buf[0]<<8|buf[1]; 
	
	//M2 motor encoder data
	i2cRead(Motor_model_ADDR, READ_TEN_M2Enconer_REG, 2, buf);
	Encoder_Offset[1] = buf[0]<<8|buf[1];
	
	//M3 motor encoder data
	i2cRead(Motor_model_ADDR, READ_TEN_M3Enconer_REG, 2, buf);
	Encoder_Offset[2] = buf[0]<<8|buf[1];
	
	//M4 motor encoder data
	i2cRead(Motor_model_ADDR, READ_TEN_M4Enconer_REG, 2, buf);
	Encoder_Offset[3] = buf[0]<<8|buf[1];
	
}

//Read the encoder data of the motor rotation
void read_all_encoder(void)
{
	static uint8_t buf[2];
	static uint8_t buf2[2];
	
	//M1 motor encoder data
	i2cRead(Motor_model_ADDR, READ_ALLHigh_M1_REG, 2, buf);
	i2cRead(Motor_model_ADDR, READ_ALLLOW_M1_REG, 2, buf2);
	
	Encoder_Now[0] = buf[0]<<24|buf[1]<<16|buf2[0]<<8|buf2[1]; 
	
	//M2 motor encoder data
	i2cRead(Motor_model_ADDR, READ_ALLHigh_M2_REG, 2, buf);
	i2cRead(Motor_model_ADDR, READ_ALLLOW_M2_REG, 2, buf2);
	Encoder_Now[1] = buf[0]<<24|buf[1]<<16|buf2[0]<<8|buf2[1];
	
	//M3 motor encoder data
	i2cRead(Motor_model_ADDR, READ_ALLHigh_M3_REG, 2, buf);
	i2cRead(Motor_model_ADDR, READ_ALLLOW_M3_REG, 2, buf2);
	Encoder_Now[2] = buf[0]<<24|buf[1]<<16|buf2[0]<<8|buf2[1];
	
	
	//M4 motor encoder data
	i2cRead(Motor_model_ADDR, READ_ALLHigh_M4_REG, 2, buf);
	i2cRead(Motor_model_ADDR, READ_ALLLOW_M4_REG, 2, buf2);
	Encoder_Now[3] = buf[0]<<24|buf[1]<<16|buf2[0]<<8|buf2[1];
	
}


void init_motor_bsp(uint8_t motor_type)
{
	if(motor_type == 1)
	{
		set_motor_type(motor_type);//Configure motor type
		delay_ms(100);
		set_pulse_phase(34);//Configure the reduction ratio. Check the motor manual to find out
		delay_ms(100);
		set_pulse_line(11);//Configure the magnetic ring wire. Check the motor manual to get the result.
		delay_ms(100);
		set_wheel_dia(67.00);//Configure the wheel diameter and measure it
		delay_ms(100);
		set_motor_deadzone(100);//Configure the motor dead zone, and the experiment shows
		delay_ms(100);
	} 
	else if(motor_type == 2)
	{
		set_motor_type(2);
		delay_ms(100);
		set_pulse_phase(34);
		delay_ms(100);
		set_pulse_line(13);
		delay_ms(100);
		set_wheel_dia(48.00);
		delay_ms(100);
		set_motor_deadzone(1300);
		delay_ms(100);
	} 
	else if(motor_type == 3)
	{
		set_motor_type(3);
		delay_ms(100);
		set_pulse_phase(45);
		delay_ms(100);
		set_pulse_line(13);
		delay_ms(100);
		set_wheel_dia(68.00);
		delay_ms(100);
		set_motor_deadzone(1250);
		delay_ms(100);
	} 
	else if(motor_type == 4)
	{
		set_motor_type(4);
		delay_ms(100);
		set_pulse_phase(48);
		delay_ms(100);
		set_motor_deadzone(1000);
		delay_ms(100);
	} 
	else if(motor_type == 5)
	{
		set_motor_type(1);
		delay_ms(100);
		set_pulse_phase(40);
		delay_ms(100);
		set_pulse_line(11);
		delay_ms(100);
		set_wheel_dia(67.00);
		delay_ms(100);
		set_motor_deadzone(1600);
		delay_ms(100);
	} 
	else
	{
		printf("motor type error, please check! \n");
		return;
	}
	
	encoder_start = read_encoder_start();
	printf("Encoder start value is %d \n",encoder_start);

}

int read_encoder_start()
{
    read_all_encoder();
    return Encoder_Now[3];
}


int32_t count_to_degree(int32_t count, uint16_t pulse_phase)
{
    return (count / pulse_phase);
}

int32_t degree_to_count(int32_t degree, uint16_t pulse_phase)
{
    return (degree * pulse_phase);
}

int calc_pwm_input(int pid_error)
{
	int pwm_input = map(pid_error, -1000, 1000, PWM_MIN, PWM_MAX);
	return constrain(pwm_input, PWM_MIN, PWM_MAX);
}

void pid_pos_control(int target_deg, int Kp, int Ki, int Kd)
{
	int t_prev = millis();
	read_all_encoder();
	int current_deg = count_to_degree(Encoder_Now[3]-encoder_start, pulse_phase);
	printf("Current position is %d degree \n", current_deg);
	printf("Target position is %d degree \n", target_deg);
	int p_error_prev = 0;
	int p_error = 0;
	int i_error=0;
	int d_error=0;
	delay_ms(10);

	while (target_deg - current_deg > 1)
	{
		int t_now = millis();
		read_all_encoder();
		current_deg = count_to_degree(Encoder_Now[3]-encoder_start, pulse_phase);
		p_error = target_deg - current_deg;
		i_error += p_error * (t_now - t_prev);
		d_error = (p_error-p_error_prev) / (t_now - t_prev);
		int pwm_input = -1*calc_pwm_input(Kp * p_error + Ki * i_error + Kd * d_error);
		printf("Current deg %d, P error %d, I error %d, D error %d, PWM input %d \n", current_deg, p_error, i_error, d_error, pwm_input);
		control_pwm(0, 0, 0, pwm_input);
		t_prev = t_now;
		p_error_prev = p_error;
		delay_ms(10);
	}
	printf("Target position %d reached! \n", target_deg);
	control_pwm(0, 0, 0, 0); // Stop the motor
	
}




