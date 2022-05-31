#include "wii_i2c.h"

// pins connected to the Nunchuk:
#define PIN_SDA 32
#define PIN_SCL 33

// ESP32 I2C port (0 or 1):
#define WII_I2C_PORT 1

void setup()
{
  Serial.begin(115200);

  if (wii_i2c_init(WII_I2C_PORT, PIN_SDA, PIN_SCL) != 0) {
    Serial.printf("Error initializing nunchuk :(");
    return;
  }
  wii_i2c_request_state();
  
}

void loop()
{

  
  const unsigned char *data = wii_i2c_read_state();
  wii_i2c_request_state();
  if (! data) {
    Serial.println("no data available :(");
  } else {
    wii_i2c_nunchuk_state state;
    wii_i2c_decode_nunchuk(data, &state);

     String x=String(state.x+128);
     String y=String(state.y+128);
     String c=String(int(state.c));
     String z=String(int(state.z));

    while (x.length()<3){
          x='0'+x;}
    while (y.length()<3){
          y='0'+y;}
        String msg = x+y+c+z;
       Serial.println(msg);
    
  }
  delay(10);
}
