/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-esp32-arduino-ide/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/




//////////////
////ESP NOW
/////////////
#include <esp_now.h>
#include <WiFi.h>
uint8_t broadcastAddress[] = {0x30, 0xc6, 0xf7, 0x05, 0x80, 0xa4};//receiver MAC address

// Structure to send data
typedef struct struct_message {
    float wL_rps_cmd;
    float wR_rps_cmd;
    float blade_rps_cmd;
} struct_message;

// Create a struct_message called myData
struct_message myData;
esp_now_peer_info_t peerInfo;
// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

//////////////
////NUCNCHUK
/////////////
#include "wii_i2c.h"

// pins connected to the Nunchuk:
#define PIN_SDA 32
#define PIN_SCL 33

// ESP32 I2C port (0 or 1):
#define WII_I2C_PORT 1


void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
 //////////////
////NUCNCHUK
/////////////

  if (wii_i2c_init(WII_I2C_PORT, PIN_SDA, PIN_SCL) != 0) {
    Serial.printf("Error initializing nunchuk :(");
    return;
  }
  wii_i2c_request_state();


//////////////
////ESPNOW
/////////////
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
//int output;
///////////////
///// Mower
///////////////

//target values
float groundspeed_fpm = 100.0; //225 should give wheel rps of 1
float tipspeed_fpm = 11080.0;


//convert target values to odrive command values
float wheel_circum_ft=45.0/12.0; //45 inches, measured
float gearbox_ratio=20.0;
float wheel_rps = groundspeed_fpm*(1.0/60.0)*(1.0/wheel_circum_ft) *gearbox_ratio;
float blade_circum_ft=3.14*(17.0/12.0); //diam is 17 inches
float blade_rps = tipspeed_fpm*(1.0/60.0)*(1.0/blade_circum_ft) ;



//////////////
////NUCNCHUK
/////////////
const unsigned char *data = wii_i2c_read_state();
  wii_i2c_request_state();
  
  if (! data) {Serial.println("no data available :(");} 
     else {
    wii_i2c_nunchuk_state state;
    wii_i2c_decode_nunchuk(data, &state);
      //by default, send nothing
      myData.wL_rps_cmd=0;
      myData.wR_rps_cmd=0;
      myData.blade_rps_cmd=0;

      //if c is pressed, allow values to be transmitted
      if (int(state.c)){
      myData.wL_rps_cmd=float(state.x/128.0*wheel_rps);
      myData.wR_rps_cmd=float(state.y/128.0*wheel_rps);
      myData.blade_rps_cmd=float(state.z*blade_rps);
  
}
}

     
 //////////////
////ESPNOW
/////////////
 // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");

  }
  else {
    Serial.println("Error sending the data");
  }
  delay(50);
}
