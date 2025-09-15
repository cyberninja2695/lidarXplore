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

#define LED_PIN LED_BUILTIN

const int HEADLIGHT_LEFT_INNER = 49;
const int HEADLIGHT_LEFT_OUTER = 52;
const int HEADLIGHT_RIGHT_OUTER = 37;
const int HEADLIGHT_RIGHT_INNER = 40;

const int BLINKER_LEFT_INNER = 53;
const int BLINKER_LEFT_OUTER = 56;
const int BLINKER_RIGHT_OUTER = 33;
const int BLINKER_RIGHT_INNER = 36;

typedef enum LED_Mode {
  LIGHTS_OFF,
  HEADLIGHT_ON,
  HEADLIGHT_OFF,
  TAILLIGHT_ON,
  TAILLIGHT_OFF,
  BLINK_LEFT_ON,
  BLINK_LEFT_OFF,
  BLINK_RIGHT_ON,
  BLINK_RIGHT_OFF,
  HAZARD_ON,
  HAZARD_OFF
};
LED_Mode led_mode = LIGHTS_OFF;

//user function declarations
void setLEDMode (int data);
void controlLED();


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
    //Serial.println("Heartbeat printed");
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
  setLEDMode(msg_led->data);
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

  delay(500);

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
  const unsigned int timer_timeout = 500;
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
  //delay(100);
  RCCHECK(rclc_executor_spin_some(&executor_pub, RCL_MS_TO_NS(100)));
  RCCHECK(rclc_executor_spin_some(&executor_sub, RCL_MS_TO_NS(100)));
  controlLED();
}












//---------------------------------------------/
/********** USER FUNCTION DEFINITION **********/
//---------------------------------------------/

void setLEDMode (int data) {
  //if(data%2 == 0) led_mode = HEADLIGHT_OFF;
  if(data == 0) led_mode = HEADLIGHT_ON;
  //else if (data%4 ==1) led_mode = BLINK_LEFT_ON;
  //else if (data%4 ==2) led_mode = BLINK_LEFT_OFF;
  //else if (data%4 ==1) led_mode = BLINK_RIGHT_ON;
  //else if (data%4 ==2) led_mode = BLINK_RIGHT_OFF;
  else if (data%2 == 0) led_mode = HAZARD_ON;
  else if (data%2 == 1) led_mode = HAZARD_OFF;
}


void controlLED() {
  if(led_mode == LIGHTS_OFF) {
    digitalWrite(LED_PIN, LOW);
    ///*
    for(int i=HEADLIGHT_LEFT_INNER; i<=BLINKER_LEFT_OUTER; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    for(int i=BLINKER_RIGHT_OUTER; i<=HEADLIGHT_RIGHT_INNER; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    //TODO: Add Tail Lights
    strip.show();
    //*/
  }

  if(led_mode == HEADLIGHT_OFF) {
    digitalWrite(LED_PIN, LOW);
    ///*
    for(int i=HEADLIGHT_LEFT_INNER; i<=HEADLIGHT_LEFT_OUTER; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    for(int i=HEADLIGHT_RIGHT_OUTER; i<=HEADLIGHT_RIGHT_INNER; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    strip.show();
    //*/
  }

  else if(led_mode == HEADLIGHT_ON) {
    digitalWrite(LED_PIN, HIGH);
    int i= 0;
    while(i < 4) {
      strip.setPixelColor(HEADLIGHT_LEFT_INNER+i, strip.Color(128, 0, 32));
      strip.setPixelColor(HEADLIGHT_RIGHT_INNER-i, strip.Color(128, 0, 32));
      i++;
      strip.show();
      delay(50);
    }
  }

  else if(led_mode == BLINK_LEFT_ON) {
    digitalWrite(LED_PIN, LOW);
    for (int i = BLINKER_LEFT_INNER; i <= BLINKER_LEFT_OUTER; i++) {
      strip.setPixelColor(i, strip.Color(255, 100, 0));
    }
    //TODO: Add Tail Blinkers
    strip.show();
  }

  else if(led_mode == BLINK_LEFT_OFF) {
    digitalWrite(LED_PIN, LOW);
    for (int i = BLINKER_LEFT_INNER; i <= BLINKER_LEFT_OUTER; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    //TODO: Add Tail Blinkers
    strip.show();
  }

  else if(led_mode == BLINK_RIGHT_ON) {
    digitalWrite(LED_PIN, LOW);
    for (int i = BLINKER_RIGHT_OUTER; i <= BLINKER_RIGHT_INNER; i++) {
      strip.setPixelColor(i, strip.Color(255, 100, 0));
    }
    //TODO: Add Tail Blinkers
    strip.show();
  }

  else if(led_mode == BLINK_RIGHT_OFF) {
    digitalWrite(LED_PIN, LOW);
    for (int i = BLINKER_RIGHT_OUTER; i <= BLINKER_RIGHT_INNER; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    //TODO: Add Tail Blinkers
    strip.show();
  }

  else if(led_mode == HAZARD_ON) {
    for (int i = BLINKER_LEFT_INNER; i <= BLINKER_LEFT_OUTER; i++) {
      strip.setPixelColor(i, strip.Color(255, 100, 0));
    }
    for (int i = BLINKER_RIGHT_OUTER; i <= BLINKER_RIGHT_INNER; i++) {
      strip.setPixelColor(i, strip.Color(255, 100, 0));
    }
    //TODO: Add Tail Blinkers
    strip.show();
  }

  else if(led_mode == HAZARD_OFF) {
    for (int i = BLINKER_LEFT_INNER; i <= BLINKER_LEFT_OUTER; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    for (int i = BLINKER_RIGHT_OUTER; i <= BLINKER_RIGHT_INNER; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    //TODO: Add Tail Blinkers
    strip.show();
  }

}