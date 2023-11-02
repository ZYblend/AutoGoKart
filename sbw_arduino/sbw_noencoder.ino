

#define LED_PIN LED_BUILTIN


#include <micro_ros_arduino.h>

#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/float32.h>
#include <geometry_msgs/msg/point_stamped.h>

rcl_subscription_t subscriber;
geometry_msgs__msg__PointStamped input;

rcl_publisher_t publisher;
std_msgs__msg__Float32 msg;
rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_timer_t timer;

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}


void error_loop(){
  while(1){
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(100);
  }
}



#define DIR_PIN 11
#define STEP_PIN 12


int relative_position = 7;
int maxLimit = 30;
int minLimit = -30;
float encoder_value = 0;
int spr = 4000;

float prev = 0;

void subscription_callback(const void * msgin){

  const geometry_msgs__msg__PointStamped * msg = (const geometry_msgs__msg__PointStamped *)msgin;
  float angle = msg->point.x;

  if(prev != angle)
  {
    if(angle > 90) angle = 90;
    if(angle < -90) angle = -90;
    
    float change = prev - angle;
    prev = angle;

    angle = change;

    digitalWrite(LED_PIN, (int(angle) > 0) ? LOW : HIGH);
    Serial.println(angle);
  
    if(angle<0)
      digitalWrite(DIR_PIN,HIGH);
    else
      digitalWrite(DIR_PIN,LOW);
      
    int step_input = map(abs(angle),0,360,0,spr);
    
    for (int i=0;i<step_input;i++){
    digitalWrite(STEP_PIN,HIGH);
    delayMicroseconds(1000);
    digitalWrite(STEP_PIN,LOW);
    delayMicroseconds(1000);
    
    }
  }
}

void setup() {

  set_microros_transports();

  delay(2000);

  allocator = rcl_get_default_allocator();

  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  RCCHECK(rclc_node_init_default(&node, "sbw", "", &support));

  RCCHECK(rclc_publisher_init_default(
    &publisher,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
    "sbw_feedback"));

  RCCHECK(rclc_subscription_init_default(
    &subscriber,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, PointStamped),
    "sbw_control"));


  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
    RCCHECK(rclc_executor_add_subscription(&executor, &subscriber, &input, &subscription_callback, ON_NEW_DATA));

  pinMode(LED_PIN, OUTPUT);
   pinMode(STEP_PIN, OUTPUT);
    pinMode(DIR_PIN, OUTPUT);
  
  Serial.begin(115200);
  
}


void loop() {
  msg.data = 1.0;
  Serial.println(msg.data);     
    RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));


    RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));

}
