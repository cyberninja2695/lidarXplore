// this code is example of publisher and subscriber
// https://github.com/micro-ROS/micro_ros_arduino/blob/galactic/examples/micro-ros_subscriber/micro-ros_subscriber.ino
// https://github.com/micro-ROS/micro_ros_arduino/blob/galactic/examples/micro-ros_publisher/micro-ros_publisher.ino
// run micro_ros_agent before connecting ESP on Serial: ros2 run micro_ros_agent micro_ros_agent serial --dev /dev/ttyUSB0
// docker run -it --rm -v /dev:/dev --privileged --net=host microros/micro-ros-agent:galactic serial --dev /dev/cu.usbmodem1422201 -v6

#include <Arduino.h>
#include <micro_ros_platformio.h>

#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/int32.h>
#include <std_msgs/msg/string.h>

#include <Adafruit_NeoPixel.h>
const int PIN_LED_DATA = GPIO_NUM_14;
int LED_BRIGHTNESS = 20;
const int NUM_LEDS = 90;

Adafruit_NeoPixel strip(NUM_LEDS, PIN_LED_DATA, NEO_GRB + NEO_KHZ800);

//user function declarations
void controlLED(int data);


rcl_node_t node;
rclc_support_t support;
rcl_allocator_t allocator;

// publisher
rcl_publisher_t publisher;
std_msgs__msg__Int32 msg_heartbeat;
rclc_executor_t executor_pub;
rcl_timer_t timer;

// subscriber
rcl_subscription_t subscriber;
std_msgs__msg__Int32 msg_led;
rclc_executor_t executor_sub;

#define LED_PIN LED_BUILTIN

#define RCCHECK(fn)              \
  {                              \
    rcl_ret_t temp_rc = fn;      \
    if ((temp_rc != RCL_RET_OK)) \
    {                            \
      error_loop();              \
    }                            \
  }
#define RCSOFTCHECK(fn)          \
  {                              \
    rcl_ret_t temp_rc = fn;      \
    if ((temp_rc != RCL_RET_OK)) \
    {                            \
    }                            \
  }

/**
 * @brief loop to indicate error with blinking LED
 *
 */
void error_loop()
{
  while (1)
  {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(100);
  }
}

void timer_callback(rcl_timer_t *timer, int64_t last_call_time)
{
  RCLC_UNUSED(last_call_time);
  if (timer != NULL)
  {
    RCSOFTCHECK(rcl_publish(&publisher, &msg_heartbeat, NULL));
    msg_heartbeat.data++;
  }
}

/**
 * @brief subscription callback executed at receiving a message
 *
 * @param msgin
 */
void subscription_callback(const void *msgin)
{
  const std_msgs__msg__Int32 *msg_led = (const std_msgs__msg__Int32 *)msgin;
  controlLED(msg_led->data);
}







void setup()
{
  Serial.begin(115200);
  set_microros_serial_transports(Serial);
  strip.setBrightness(LED_BRIGHTNESS);
  strip.begin();

  /*
  IPAddress agent_ip(192, 168, 1, 113);
  size_t agent_port = 8888;

  char ssid[] = "WIFI_SSID";
  char psk[]= "WIFI_PSK";

  set_microros_wifi_transports(ssid, psk, agent_ip, agent_port);
  */
  

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  delay(2000);

  allocator = rcl_get_default_allocator();

  // create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // create node
  RCCHECK(rclc_node_init_default(&node, "led_control_node", "", &support));

  // create subscriber
  // const char topic_name_led[] = "xiao_led_state";
  RCCHECK(rclc_subscription_init_default(
      &subscriber,
      &node,
      //ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
      ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
      "lidarxplore_heartbeat"));

  // create publisher
  // const char topic_name_heatbeat[] = "xiao_heartbeat";
  RCCHECK(rclc_publisher_init_default(
      &publisher,
      &node,
      ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
      "lidarxplore_heartbeat"));

  // create timer, called every 1000 ms to publish heartbeat
  const unsigned int timer_timeout = 1000;
  RCCHECK(rclc_timer_init_default(
      &timer,
      &support,
      RCL_MS_TO_NS(timer_timeout),
      timer_callback));

  // create executor
  RCCHECK(rclc_executor_init(&executor_pub, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_timer(&executor_pub, &timer));

  RCCHECK(rclc_executor_init(&executor_sub, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_subscription(&executor_sub, &subscriber, &msg_led, &subscription_callback, ON_NEW_DATA));

  msg_heartbeat.data = 0;
}

void loop()
{
  delay(100);
  RCCHECK(rclc_executor_spin_some(&executor_pub, RCL_MS_TO_NS(100)));
  RCCHECK(rclc_executor_spin_some(&executor_sub, RCL_MS_TO_NS(100)));
}












//---------------------------------------------/
/********** USER FUNCTION DEFINITION **********/
//---------------------------------------------/

void controlLED(int data) {
  if(data%2 == 0) {
    digitalWrite(LED_PIN, LOW);
    ///*
    //for(int i=37; i<42; i++) {
    for(int i=37; i<38; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    //for(int i=49; i<54; i++) {
    for(int i=52; i<53; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    //*/

  }
  else if(data%2 != 0) {
    digitalWrite(LED_PIN, HIGH);
    ///*
    //for(int i=37; i<42; i++) {
    for(int i=37; i<38; i++) {
      strip.setPixelColor(i, strip.Color(0, 255, 0));
    }
    //for(int i=49; i<54; i++) {
    for(int i=52; i<53; i++) {
      strip.setPixelColor(i, strip.Color(0, 255, 0));
    }
    //*/
 
  }
  strip.show();
}