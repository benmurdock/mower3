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
struct_message myData;


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
ODriveArduino odrive_blades(Serial1);
ODriveArduino odrive_wheels(Serial2);
unsigned long t0;//timer for checking message delay
unsigned long max_allowable_delay=500;
bool watchdog_flag=false;
void setup() {

  //////
//esp now
///////

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
  
//////////
///odrive
/////

  // Set up 2 additional serial channels to the odrive boards
  Serial1.begin(BAUDRATE, SERIAL_8N1, ESP32_UART2_PIN_TX, ESP32_UART2_PIN_RX);
  Serial2.begin(BAUDRATE, SERIAL_8N1, ESP32_UART_Bonus_PIN_TX, ESP32_UART_Bonus_PIN_RX);
  while (!Serial) ;  // wait for Arduino Serial 0 Monitor to open
  while (!Serial1) ; // wait for Arduino Serial 1 
  while (!Serial2) ; // wait for Arduino Serial 2 
  
  // Calibrate axes and set to move
  set_state_allMotors(AXIS_STATE_FULL_CALIBRATION_SEQUENCE , true);
  delay(15000);
  set_state_allMotors(AXIS_STATE_CLOSED_LOOP_CONTROL , false);
  t0=millis();
  
}//end void setup()

/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////        MAIN LOOP       ////////////////////////////////////////////
void loop() {
  if(millis()-t0>max_allowable_delay){
    enable_all_watchdogs(false);
    watchdog_flag=false;
    set_blade_vel(0);
    set_wheel_vel(0,0);
  }

}
/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////        FXNS FOR ODRIVE COMMS       ////////////////////////////////////////////
void set_state_allMotors(int requested_state, bool shouldWait){
      Serial << "Requesting all motor calibration" << '\n';
        odrive_wheels.run_state(0, requested_state, false);
        odrive_wheels.run_state(1, requested_state, false);
        odrive_blades.run_state(0, requested_state, false);
        odrive_blades.run_state(1, requested_state, shouldWait);}

void set_blade_vel(int blade_RPS){
      Serial << "Commanding Blade Spin" << '\n';
        odrive_blades.SetVelocity(0, blade_RPS);
        odrive_blades.SetVelocity(1, blade_RPS);}

void set_wheel_vel(float leftWheel_RPS,float rightWheel_RPS){
      Serial << "Commanding Wheel Spin" << '\n';
        odrive_wheels.SetVelocity(0, leftWheel_RPS);
        odrive_wheels.SetVelocity(1, rightWheel_RPS);}

void enable_all_watchdogs(bool should_enable){
      odrive_blades.EnableWatchdog(0,should_enable);
      odrive_blades.EnableWatchdog(1,should_enable);
      odrive_wheels.EnableWatchdog(0,should_enable);
      odrive_wheels.EnableWatchdog(1,should_enable);}

void clear_axis_errors(){
      odrive_blades.ClearAxisError(0);
      odrive_blades.ClearAxisError(1);
      odrive_wheels.ClearAxisError(0);
      odrive_wheels.ClearAxisError(1);}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
      memcpy(&myData, incomingData, sizeof(myData)); //dunno what this does
      t0=millis(); //reset local watchdog    
      if (watchdog_flag==false){
        watchdog_flag=true;
      enable_all_watchdogs(true);}

      
      set_blade_vel(float(myData.blade_rps_cmd));
      set_wheel_vel(float(myData.wL_rps_cmd) , float(myData.wR_rps_cmd));}
