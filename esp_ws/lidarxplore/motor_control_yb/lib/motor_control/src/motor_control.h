
#include <Arduino.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


extern int encoder_start;

void read_encoder_start();
int32_t count_to_degree(int32_t count, uint16_t pulse_phase);
int32_t degree_to_count(int32_t degree, uint16_t pulse_phase);
int calc_pid_error(int target_pos, int start_pos);
int calc_pwm_input(int pid_error);