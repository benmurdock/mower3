/*
 * We are going to use the ESP32 extra serial communication from HardwareSerial
 * ** https://hackaday.com/2017/08/17/secret-serial-port-for-arduinoesp32/
 * ** https://youtu.be/GwShqW39jlE
 * 
 * Much of the code is from the Example file in ODriveArduino Examples: ODriveArduinoTest
 */
//////////////////////////////////////////////////////////////////////////////////////////
///////////////From herer https://discourse.odriverobotics.com/t/esp32-odrive-communication-solved/3422/4
/////////////////////////

//////
//esp now
///////

#include <esp_now.h>
#include <WiFi.h>

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    float wL_rps_cmd;
    float wR_rps_cmd;
    float blade_rps_cmd;
    
} struct_message;

// Create a struct_message called myData
struct_message myData;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
 
  Serial.print(myData.wL_rps_cmd);
  Serial.print(" , ");
  Serial.print(myData.wR_rps_cmd);
  Serial.print(" , ");
  Serial.println(myData.blade_rps_cmd);
      
      ///this is where I will make a call to some function to set speeds
  set_blade_vel(float(myData.blade_rps_cmd));
  set_wheel_vel(float(myData.wL_rps_cmd) , float(myData.wR_rps_cmd));

}




//////////
///odrive
/////
#include <ODriveArduino.h>
#define ESP32_UART2_PIN_TX 17
#define ESP32_UART2_PIN_RX 16 
#define ESP32_UART_Bonus_PIN_TX 4
#define ESP32_UART_Bonus_PIN_RX 2
#define BAUDRATE 115200 // ODrive uses 115200 baud

// Printing with stream operator
template<class T> inline Print& operator <<(Print &obj,     T arg) { obj.print(arg);    return obj; }
template<>        inline Print& operator <<(Print &obj, float arg) { obj.print(arg, 4); return obj; }

// ODrive object //HardwareSerial Serial1;
ODriveArduino odrive_blades(Serial1);
ODriveArduino odrive_wheels(Serial2);
float vel_limit = 22000.0f;
float current_lim = 11.0f;

void setup() {

  //////
//esp now
///////
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
//////////
///odrive
/////
  
  // Serial to PC
  //Serial.begin(BAUDRATE); //already done from espnow section
  
  // Set up 2 additional serial channels to the odrive boards
  Serial1.begin(BAUDRATE, SERIAL_8N1, ESP32_UART2_PIN_TX, ESP32_UART2_PIN_RX);
  Serial2.begin(BAUDRATE, SERIAL_8N1, ESP32_UART_Bonus_PIN_TX, ESP32_UART_Bonus_PIN_RX);
  
  while (!Serial) ;  // wait for Arduino Serial 0 Monitor to open
  while (!Serial1) ; // wait for Arduino Serial 1 
  
// Calibrate axes and set to move
  //delay(3000); //give me a chance to lift wheels before calibration
  set_state_allMotors(AXIS_STATE_MOTOR_CALIBRATION , true);
  set_state_allMotors(AXIS_STATE_ENCODER_OFFSET_CALIBRATION , true);
  set_state_allMotors(AXIS_STATE_CLOSED_LOOP_CONTROL , false);

//enable watchdog and enter main loop
enable_all_watchdogs();
}

void loop() {
  
}

void set_state_allMotors(int requested_state, bool shouldWait){
      Serial << "Requesting all motor calibration" << '\n';
        odrive_wheels.run_state(0, requested_state, false);
                delay(50);
        odrive_wheels.run_state(1, requested_state, false);
                delay(50);
        odrive_blades.run_state(0, requested_state, false);
                delay(50);
        odrive_blades.run_state(1, requested_state, shouldWait);
}

void set_blade_vel(int blade_RPS){
      Serial << "Commanding Blade Spin" << '\n';
        
        odrive_blades.SetVelocity(0, blade_RPS);
                delay(50);
        odrive_blades.SetVelocity(1, blade_RPS);
}
void set_wheel_vel(float leftWheel_RPS,float rightWheel_RPS){
      Serial << "Commanding Wheel Spin" << '\n';
        
        odrive_wheels.SetVelocity(0, leftWheel_RPS);
        odrive_wheels.SetVelocity(1, rightWheel_RPS);
}


void enable_all_watchdogs(){
  odrive_blades.EnableWatchdog(0,.10,true);
  odrive_blades.EnableWatchdog(1,.10,true);
  odrive_wheels.EnableWatchdog(0,.10,true);
  odrive_wheels.EnableWatchdog(1,.10,true);
}
