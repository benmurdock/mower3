#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h> //enable spi for screen communication  
#include <string.h>//plan to convert floats to strings and pad with " " to eliminate extra pixels

////////////////
///////  Joysticks (2 variable(s): navjoy and drivejoy)
////////////////
struct joystick
{
  int XPIN, YPIN; //gpio for input from joystick
  int X0, Y0, xval, yval;   //calibration and current values
  int THRESH_MOVE, THRESH_ZERO; //min to count as move and max to count as return to center
};

joystick navjoy={36, 39, 0, 0, 0, 0, 200, 1000}; //joystick for navigating menu
joystick drivejoy;//{XPIN?, YPIN?, 0, 0, 0, 0, 200, 1000, 0}; //joystick for controlling drive wheels

////////////////
///////  Screen (3 variable(s): current, grid, tft)
////////////////
struct current
{
  int page; //which layout template gets sent to display
  int row; //which row on layout is highlighted
  int col; //which row on layout is highlighted
  int flexvar1; //Flexible meaning based on context (Drivemode in dashboard, etc.)
  int flexvar2; //Flexible meaning (used as old drivestatus in dashboard)
};
current current;

struct grid
{
int x[4] = {0,17,125,180};  //x coords of interest
int y[4] = {17,90,158,230}; //y coord of section headings
int ROWHEIGHT = 16;        //px height for row
};
grid grid; //use for coordinates of interest when laying out dashboard

TFT_eSPI tft = TFT_eSPI(); // create screen to receive draw commands

////////////////
///////  Mower (2 variable(s): tx_msg and rx_msg)
////////////////
struct tx_msg
{
  int com_wheel_l, com_wheel_r, com_blade; // (current) commands from controller
  int target_wheel, target_blade, target_height; //target maximum values (mostly extraneous for tx, but had to put somewhere)
  float volts_controller; //don't need to tx, but again, needs to go somewhere
};
tx_msg tx_msg={0,0,0,200,12000,45,12.1};

struct rx_msg
{
  float speed_bl, speed_br, power_bl, power_br;//blade speed and power
  float speed_wl, speed_wr; //wheel speed
  float volts_mower;
};
rx_msg rx_msg={12.1, 12.0, 435, 469, 201,150, 49.5};

////////////////
///////  Dashboard data aggregator (2 variable(s): data_color, dashsection)
////////////////
int data_color = TFT_YELLOW;
struct dashsection
  {
    char title[10];
    char label_col1[11]; 
    char label_col2[11];
    char label_row1[11];
    char label_row2[11];
    float data_r1c1;
    float data_r1c2;
    float data_r2c1;
    float data_r2c2;
  };
dashsection dashsection[]=
  {
    {"READY","","","","",0,0,0,0},
    {"BLADES","(kfpm)","(W)","Left","Right",rx_msg.speed_bl,rx_msg.power_bl,rx_msg.speed_br,rx_msg.power_br},
    {"WHEELS","(fpm)","","Left","Right",rx_msg.speed_wl,-1,rx_msg.speed_wr,-1},
    {"BATTERY","(V)","(%)","Controller","Motor",12.1,93,rx_msg.volts_mower,82}
  };

void setup() 
{
  Serial.begin(9600);
  //joystick zero calibration (center stick before running)
  navjoy.X0 = analogRead(navjoy.XPIN);
  navjoy.Y0 = analogRead(navjoy.YPIN);

  //screen setup
  tft.begin();
  tft.setRotation(2);
  tft.fillScreen(TFT_BLACK);
  current.page = 0;

  //set dashboard to drigger full draw
  current.flexvar2 = -1;
  current.flexvar1 = 0; 
}

void loop() {
  if(current.page == 0)
  {
    draw_menu();
    navjoyread_menu();
  }

  if (current.page == 1)
  {
    draw_dash(); 
    navjoyread_dash(); 
  }

  //listen for new message

  //update values from rx msg in 
    //
}

int batt_est(int volts)
{
  return 93;
}
