/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-esp32-arduino-ide/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/
        ///////////ESP Now ///////////
                    #include <esp_now.h>
                    #include <WiFi.h>
                    // REPLACE WITH YOUR RECEIVER MAC Address
                    //board#2 MAC: 30:c6:f7:05:80:a4
                    uint8_t broadcastAddress[] = {0x30, 0xc6, 0xf7, 0x05, 0x80, 0xa4};
                    
                    // Structure example to send data
                    // Must match the receiver structure
                    typedef struct struct_message {
                      float wheelSpeed_L;
                      float wheelSpeed_R;
                      float bladeSpeed;
                      float moveDeck;
                    } struct_message;
                    
                    struct_message myData;// Create a struct_message called myData
                    esp_now_peer_info_t peerInfo;
                    // callback when data is sent
                    void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
                    Serial.print("\r\nLast Packet Send Status:\t");
                    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
                     }

        /////////// INPUTS  ///////////
                      #define JoyStick_X 36
                      #define JoyStick_Y 39
                      #define Button_Activate 27
                      #define Button_SpinBlades 14
                      bool offsetFlag = false;
                      float x_zero;
                      float y_zero;

                    

 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
 
            ///////////ESP Now ///////////
                          WiFi.mode(WIFI_STA); // Set device as a Wi-Fi Station
                          if (esp_now_init() != ESP_OK) { // Init ESP-NOW
                            Serial.println("Error initializing ESP-NOW");
                            return;
                          }
                          esp_now_register_send_cb(OnDataSent);// get the status of Trasnmitted packet
                          memcpy(peerInfo.peer_addr, broadcastAddress, 6); // Register peer
                          peerInfo.channel = 0;  
                          peerInfo.encrypt = false;
                          
                          // Add peer        
                          if (esp_now_add_peer(&peerInfo) != ESP_OK){
                            Serial.println("Failed to add peer");
                            return;
                          } 
            /////////// INPUTS  ///////////
                          pinMode(JoyStick_X, INPUT);
                           pinMode(JoyStick_Y, INPUT);
                           pinMode(Button_Activate, INPUT_PULLUP);
                           pinMode(Button_SpinBlades, INPUT_PULLUP);
}
 
void loop() {
             delay(1000);//// delete this later
            /////////// GET INPUTS  ///////////
                float x_in = analogRead(JoyStick_X);
                float y_in = analogRead(JoyStick_Y); 
                float val_Button_Activate = -(1-digitalRead(Button_Activate));  
                float val_Button_SpinBlades = -(1-digitalRead(Button_SpinBlades));  
                
                int adc_MaxVal=4095.0; //max possible value from adc
                int adc_MinVal=0.0; //min possible value from adc
                if(offsetFlag==false){  //save first values as zero
                        x_zero=x_in*1.0;
                        y_zero=y_in*1.0;
                        offsetFlag=true;
                        }
                /// scale inputs around zero values between -100 and 100
                /// map function returns integer, so mapping -1:1 doesn't work well
                if (x_in <= x_zero){x_in = map(x_in, adc_MinVal, x_zero, -100, 0);}
                if (x_in > x_zero){x_in = map(x_in, x_zero,adc_MaxVal, 0, 100);}
                if (y_in <= x_zero){y_in = map(y_in, adc_MinVal, y_zero, -100, 0);}
                if (y_in > x_zero){y_in = map(y_in, y_zero,adc_MaxVal, 0, 100);}
            
                
                // ignore values too small to indicate intentional input
                if (abs(x_in) < 10){x_in = 0;}
                if (abs(y_in) < 10){y_in = 0;}
  
  
             //////////assign inputs ///////////

                myData.wheelSpeed_L = x_in*val_Button_Activate;
                myData.wheelSpeed_R = y_in*val_Button_Activate;
                myData.bladeSpeed = 12000*val_Button_Activate*val_Button_SpinBlades;
                myData.moveDeck = -1;
                
                printPacket(myData);
  
            //////// SEND WITH ESP Now ///////////
                esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData)); // send message
                 
                if (result == ESP_OK) {Serial.println("Sent with success");}
                else {Serial.println("Error sending the data");}
               
}



void printPacket(struct_message myData){
Serial.print(myData.wheelSpeed_L);
Serial.print(" : ");
Serial.print(myData.wheelSpeed_R);
Serial.print(" : ");
Serial.print(myData.bladeSpeed);
Serial.print(" : ");
Serial.print(myData.moveDeck);
Serial.println(" : ");

}

void get_y(){
  
  
  }
